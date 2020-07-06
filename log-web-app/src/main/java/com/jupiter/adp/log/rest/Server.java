package com.jupiter.adp.log.rest;

import static spark.Spark.get;
import static spark.Spark.port;
import static spark.Spark.post;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.attribute.BasicFileAttributes;
import java.nio.file.attribute.FileTime;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.time.Instant;
import java.time.temporal.ChronoUnit;
import java.util.Date;

import org.eclipse.jetty.http.HttpStatus.Code;
import org.eclipse.jetty.http.MimeTypes.Type;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import spark.Request;
import spark.Response;
import spark.Spark;
import spark.utils.StringUtils;

import com.jupiter.adp.log.rest.api.Event;
import com.jupiter.adp.log.rest.api.Events;
import com.jupiter.adp.log.rest.api.LevelEnum;
import com.jupiter.adp.log.rest.api.Message;
import com.jupiter.adp.log.rest.api.SourceSyntaxEnum;
import com.jupiter.adp.log.rest.api.Status;
import com.jupiter.adp.log.rest.api.StatusEnum;
import com.google.gson.Gson;
import com.google.gson.JsonSyntaxException;

public class Server {
    private static final Logger LOGGER = LoggerFactory.getLogger(Server.class);

    /*
     * REST endpoint constants.
     */
    private static final String ENDPOINT_BASE = "/log/api";
    private static final String ENDPOINT_EVENTS = "/events";
    private static final String ENDPOINT_HEALTH = "/health";

    private static final String ENDPOINT_V1_BASE = ENDPOINT_BASE + "/v1";
    private static final String ENDPOINT_V1_EVENTS = ENDPOINT_V1_BASE
            + ENDPOINT_EVENTS;
    private static final String ENDPOINT_V1_HEALTH = ENDPOINT_V1_BASE
            + ENDPOINT_HEALTH;

    /*
     * REST API constants.
     */
    private static final String LOG_PLANE_PATTERN = "^[a-z]+$";
    private static final String LOG_PLANE_DEFAULT = "restlogs";
    private static final String[] DATE_TIME_PATTERN = {
            "yyyy-MM-dd'T'HH:mm:ss.SSSZZZZZ", "yyyy-MM-dd'T'HH:mm:ssZZZZZ" };
    private static final String SOURCE_SYNTAX_DEFAULT = SourceSyntaxEnum.Unspecified
            .toString();
    private static final String LEVEL_DEFAULT = LevelEnum.info.toString();
    private static final int RC_SUCCESS = 0;
    private static final String CMD_S6_SVSTAT_LOGSTASH = "s6-svstat /var/run/s6/services/logstash";
    private static final String S6_SERVICE_UP = "up";

    /*
     * Spark constants.
     */
    private static final String ENV_VAR_SPARK_PORT = "SPARK_PORT";
    private static final int DEF_SPARK_PORT = 5080;

    /*
     * Logstash TCP input plugin constants.
     */
    private static final String ENV_VAR_LOGSTASH_INPUT_TCP_HOST = "LOGSTASH_INPUT_TCP_HOST";
    private static final String ENV_VAR_LOGSTASH_INPUT_TCP_PORT = "LOGSTASH_INPUT_TCP_PORT";
    private static final String DEF_LOGSTASH_INPUT_TCP_HOST = "localhost";
    private static final int DEF_LOGSTASH_INPUT_TCP_PORT = 7170;
    private static final String LOGSTASH_CODEC_JSON_LINES_DELIMITER = "\n";

    /*
     * Logstash health check constants
     */
    private static final String ENV_VAR_LOGSTASH_CONF = "FILE_LOGSTASH_CONF";
    private static final String DEF_LOGSTASH_CONF = "/opt/logstash/config/logstash.conf";
    private static final int LOGSTASH_CONF_CREATION_TIME_DEVIATION = 5; // seconds
    private static final String LOGSTASH_CONFIG_FIELD_OUTPUT = "output {";
    private static final String LOGSTASH_CONFIG_FIELD_ELASTICSEARCH = "elasticsearch {";
    private static final String LOGSTASH_CONFIG_FIELD_HOSTS = "hosts => ";

    /*
     * Logstash elasticsearch discovery constants
     */
    private static final String ENV_VAR_SD_ADDR = "SD_SERVER_ADDR";    
    private static final String ENV_VAR_ES_HOSTS = "ELASTICSEARCH_HOSTS";
    private static final String ES_FLAG = "SD_ELASTICSEARCH_ENABLED";
    private static final String ENV_VAR_RAW_OUTPUT = "FILE_OUTPUT_RAW_ENABLED";

    /*
     * Checks whether timestamp conforms to RFC 3339. Implementation is based on
     * https://gist.github.com/oseparovic/d9ee771927ac5f3aefc8ba0b99c0cf38.
     */
    private static boolean isValidTimestamp(String timestamp) {
        if (StringUtils.isEmpty(timestamp))
            return false;

        /*
         * Java doesn't properly handle 'Z' literal in patterns
         * "yyyy-MM-dd'T'HH:mm:ss'Z'" and "yyyy-MM-dd'T'HH:mm:ss.SSS'Z'", so
         * replace it manually with UTC time.
         */
        if (timestamp.contains("Z"))
            timestamp = timestamp.replaceAll("Z$", "+0000");

        for (String i : DATE_TIME_PATTERN)
            try {
                new SimpleDateFormat(i).parse(timestamp);
                return true;
            } catch (ParseException e) {
            }

        return false;
    }

    private static String executeCommand(String command) {
        BufferedReader reader = null;

        try {
            LOGGER.debug("command <{}>", command);

            if (StringUtils.isEmpty(command))
                return null;

            Process process = Runtime.getRuntime().exec(command);
            process.waitFor();
            int exitValue = process.exitValue();

            reader = new BufferedReader(new InputStreamReader(
                    process.getInputStream()));
            String output = "";
            String line = "";

            while ((line = reader.readLine()) != null)
                output += (line + System.lineSeparator());

            LOGGER.info("command <{}>, exitValue <{}>, output <{}>", command,
                    exitValue, output);

            if (exitValue != RC_SUCCESS)
                return null;

            return output;
        } catch (Exception e) {
            LOGGER.error("Encountered exception <{}>", e.getMessage());
            return null;
        } finally {
            try {
                if (reader != null)
                    reader.close();
            } catch (IOException e) {
            }
        }
    }

    private static boolean isLogstashConfEsHostsConfigured(Path pathLogstashConf) {
        try {
            String stringLogstashConfig = new String(
                    Files.readAllBytes(pathLogstashConf));

            int i = stringLogstashConfig.indexOf(LOGSTASH_CONFIG_FIELD_OUTPUT);
            if (i < 0) {
                LOGGER.error("Unable to find field <{}> in file <{}>.",
                        LOGSTASH_CONFIG_FIELD_OUTPUT, pathLogstashConf);
                return false;
            }

            i = stringLogstashConfig.indexOf(
                    LOGSTASH_CONFIG_FIELD_ELASTICSEARCH, i);
            if (i < 0) {
                LOGGER.warn("Unable to find field <{}> in file <{}>.",
                        LOGSTASH_CONFIG_FIELD_ELASTICSEARCH, pathLogstashConf);
                return false;
            }

            i = stringLogstashConfig.indexOf(LOGSTASH_CONFIG_FIELD_HOSTS, i);
            if (i < 0) {
                LOGGER.warn("Unable to find field <{}> in file <{}>.",
                        LOGSTASH_CONFIG_FIELD_HOSTS, pathLogstashConf);
                return false;
            }

            int j = stringLogstashConfig.indexOf('\n', i);
            if (j < 0)
                LOGGER.warn("Unable to find EOL for field <{}> in file <{}>.",
                        LOGSTASH_CONFIG_FIELD_HOSTS, pathLogstashConf);
            else
                LOGGER.debug("Found Elasticsearch hosts <{}> in file <{}>.",
                        stringLogstashConfig.substring(
                                (i + LOGSTASH_CONFIG_FIELD_HOSTS.length()), j),
                        pathLogstashConf);

            return true;
        } catch (IOException e) {
            LOGGER.error("Unable to read file <{}>; exception <{}>.",
                    pathLogstashConf, e.getMessage());
            return false;
        }
    }

    private static boolean isLogstashConfReloading(Path pathLogstashConf) {
        try {
            BasicFileAttributes attributes = Files.readAttributes(
                    pathLogstashConf, BasicFileAttributes.class);
            FileTime creationTime = attributes.creationTime();
            long diff = ChronoUnit.SECONDS.between(creationTime.toInstant(),
                    Instant.now());

            return (diff <= LOGSTASH_CONF_CREATION_TIME_DEVIATION);
        } catch (IOException e) {
            return false;
        }
    }

    private static Status getStatusServiceLog() {
        Status status = new Status();
        status.status = StatusEnum.red;

        String output = executeCommand(CMD_S6_SVSTAT_LOGSTASH);
        if (StringUtils.isEmpty(output))
            return status;

        if (!output.startsWith(S6_SERVICE_UP))
            return status;

        String value = System.getenv(ENV_VAR_LOGSTASH_CONF);
        String logstashConf = ((!StringUtils.isEmpty(value)) ? (value)
                : (DEF_LOGSTASH_CONF));
        Path pathLogstashConf = Paths.get(logstashConf);

        if (!Files.exists(pathLogstashConf))
            return status;
        
        Path pathReadiness = Paths.get("/tmp/logstash/readiness.txt");
 
        //if (!Files.exists(pathReadiness)) {
        //    status.status = StatusEnum.yellow;
        //    return status;
        //}
	
	String sdAddr = System.getenv(ENV_VAR_SD_ADDR);
        String esHosts = System.getenv(ENV_VAR_ES_HOSTS);
        String rawOutput = System.getenv(ENV_VAR_RAW_OUTPUT);
        String esFlag =  System.getenv(ES_FLAG);

        if (!Boolean.parseBoolean(esFlag)) {
            status.status = StatusEnum.green;
            return status;
        }

        if (StringUtils.isEmpty(sdAddr) && StringUtils.isEmpty(esHosts) && !StringUtils.isEmpty(rawOutput)) {
            status.status = StatusEnum.green;
            return status;
        }

        if (!isLogstashConfEsHostsConfigured(pathLogstashConf))
            return status;

        status.status = ((isLogstashConfReloading(pathLogstashConf)) ? (StatusEnum.yellow)
                : (StatusEnum.green));
        return status;
    }

    private static void sendLogstash(Events events, String host, int port) {
        try {
            SocketChannel client = SocketChannel.open(new InetSocketAddress(
                    host, port));

            for (Event i : events.events)
                try {
                    LOGGER.debug("Sending event <{}> to Logstash.",
                            i.toStringSendLogstash());
                    client.write(ByteBuffer.wrap((new Gson().toJson(i) + LOGSTASH_CODEC_JSON_LINES_DELIMITER)
                            .getBytes()));
                } catch (IOException e) {
                    LOGGER.error(
                            "Unable to send event <{}> to Logstash <{}:{}>; exception <{}>.",
                            i.toStringSendLogstash(), host, port,
                            e.getMessage());
                }

            client.close();
        } catch (IOException e) {
            LOGGER.error("I/O error <{}>.", e.getMessage());
        }
    }

    private static String processMsgEvents(Request request, Response response,
            String host, int port) {
        String message = null;

        response.type(Type.APPLICATION_JSON.toString());
        response.status(Code.BAD_REQUEST.getCode());

        Events events = null;
        try {
            events = new Gson().fromJson(request.body(), Events.class);
        } catch (JsonSyntaxException e) {
            message = "Malformed JSON";

            LOGGER.error("{}; exception <{}>.", message, e.getMessage());
            return new Gson().toJson(new Message(message));
        }

        if (events == null) {
            message = "Empty request body";

            LOGGER.error("{}; events <{}>.", message, events);
            return new Gson().toJson(new Message(message));
        }

        if (events.logPlane == null)
            events.logPlane = LOG_PLANE_DEFAULT;

        if (!events.logPlane.matches(LOG_PLANE_PATTERN)) {
            message = "Invalid logPlane";

            LOGGER.error("{} <{}>.", message, events.logPlane);
            return new Gson().toJson(new Message(message));
        }

        if (events.events == null) {
            message = "events array";

            LOGGER.error("Nonexistent {} <{}>.", message, events.events);
            return new Gson().toJson(new Message("Empty " + message));
        }

        if (events.events.isEmpty()) {
            message = "Empty events array";

            LOGGER.error("{} <{}>.", message, events.events);
            return new Gson().toJson(new Message(message));
        }

        for (Event i : events.events) {
            if (StringUtils.isEmpty(i.message)) {
                message = "Invalid event message";

                LOGGER.error("{} <{}>.", message, i.message);
                return new Gson().toJson(new Message(message));
            }

            i.logPlane = events.logPlane;

            if (i.dateTime == null)
                i.dateTime = new SimpleDateFormat(DATE_TIME_PATTERN[0])
                        .format(new Date());

            if (!isValidTimestamp(i.dateTime)) {
                message = "Invalid event dateTime";

                LOGGER.error("{} <{}>.", message, i.dateTime);
                return new Gson().toJson(new Message(message));
            }

            if ((i.source != null) && (i.source.isEmpty())) {
                message = "Invalid event source";

                LOGGER.error("{} <{}>.", message, i.source);
                return new Gson().toJson(new Message(message));
            }

            if (i.sourceSyntax == null)
                i.sourceSyntax = SOURCE_SYNTAX_DEFAULT;

            try {
                SourceSyntaxEnum.valueOf2(i.sourceSyntax);
            } catch (IllegalArgumentException e) {
                message = "Invalid event sourceSyntax";

                LOGGER.error("{} <{}>.", message, i.sourceSyntax);
                return new Gson().toJson(new Message(message));
            }

            if (i.level == null)
                i.level = LEVEL_DEFAULT;

            try {
                LevelEnum.valueOf(i.level);
            } catch (IllegalArgumentException e) {
                message = "Invalid event level";

                LOGGER.error("{} <{}>.", message, i.level);
                return new Gson().toJson(new Message(message));
            }
        }

        sendLogstash(events, host, port);

        response.status(Code.CREATED.getCode());
        message = "Received " + events.events.size() + " event(s)";

        LOGGER.info("{}.", message);
        return new Gson().toJson(new Message(message));
    }

    private static String processMsgHealth(Request request, Response response) {
        Status status = getStatusServiceLog();

        response.type(Type.APPLICATION_JSON.toString());
        switch (status.status) {
        case green:
            response.status(Code.OK.getCode());
            break;

        case yellow:
            response.status(Code.TOO_MANY_REQUESTS.getCode());
            break;

        case red:
        default:
            response.status(Code.SERVICE_UNAVAILABLE.getCode());
        }

        return new Gson().toJson(status);
    }

    public static void main(String[] args) {
        Runtime.getRuntime().addShutdownHook(new Thread() {
            public void run() {
                LOGGER.info("Shutdown hook called.");

                Spark.stop();
            }
        });

        String value = System.getenv(ENV_VAR_SPARK_PORT);
        int portSpark = ((!StringUtils.isEmpty(value)) ? (Integer
                .parseInt(value)) : (DEF_SPARK_PORT));

        value = System.getenv(ENV_VAR_LOGSTASH_INPUT_TCP_HOST);
        String hostLogstash = ((!StringUtils.isEmpty(value)) ? (value)
                : (DEF_LOGSTASH_INPUT_TCP_HOST));

        value = System.getenv(ENV_VAR_LOGSTASH_INPUT_TCP_PORT);
        int portLogstash = ((!StringUtils.isEmpty(value)) ? (Integer
                .parseInt(value)) : (DEF_LOGSTASH_INPUT_TCP_PORT));

        port(portSpark);

        post(ENDPOINT_V1_EVENTS,
                (request, response) -> {
                    return processMsgEvents(request, response, hostLogstash,
                            portLogstash);
                });

        get(ENDPOINT_V1_HEALTH, (request, response) -> {
            return processMsgHealth(request, response);
        });
    }
}
