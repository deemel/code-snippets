package com.jupiter.adp.sd;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;
import java.util.Properties;
import java.util.UUID;

import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.ecwid.consul.v1.ConsulClient;
import com.ecwid.consul.v1.QueryParams;
import com.ecwid.consul.v1.Response;
import com.ecwid.consul.v1.agent.model.NewService;
import com.ecwid.consul.v1.health.model.HealthService;
import com.ecwid.consul.v1.health.model.HealthService.Service;
import com.google.gson.Gson;

public class Client {
    private static final Logger LOGGER = LoggerFactory.getLogger(Client.class);

    private static final int RC_SUCCESS = 0;
    private static final int RC_FAILURE = 1;

    /*
     * SD (Consul) environment variables.
     */
    private static final String ENV_VAR_SD_SERVER_ADDR = "SD_SERVER_ADDR";
    private static final String ENV_VAR_SD_SERVER_PORT = "SD_SERVER_PORT";

    /*
     * Service (e.g., Elasticsearch, Log) environment variables.
     */
    private static final String ENV_VAR_SERVICE_NAME = "SERVICE_NAME";
    private static final String ENV_VAR_INSTANCE_ID = "INSTANCE_ID";
    private static final String ENV_VAR_APP_NAME = "APP_NAME";
    private static final String ENV_VAR_EXT_SERVICE_IP = "EXT_SERVICE_IP";
    private static final String ENV_VAR_EXT_SERVICE_PORT = "EXT_SERVICE_PORT";
    private static final String ENV_VAR_SD_HEALTH_CHECK_URL = "SD_HEALTH_CHECK_URL";
    private static final String ENV_VAR_SD_HEALTH_CHECK_INTERVAL = "SD_HEALTH_CHECK_INTERVAL";

    /*
     * Service defaults.
     */
    private static final int DEF_SD_SERVER_PORT = 8500;
    private static final String DEF_APP_NAME = "default";
    private static final String DEF_SD_HEALTH_CHECK_INTERVAL = "10s";

    /*
     * Service Management Agent tool (i.e., service-management-agent.sh)
     * constants.
     */
    private static final String SCRIPT_NAME = "service-management-agent.sh";
    private static final String TIMESTAMP_FORMAT = "yyyyMMddHHmmss";
    private static final String TIMESTAMP_FORMAT_STDOUT = "'['yyyy-MM-dd'T'HH:mm:ss,SSS']'";
    private static final String LOGSTASH_CONFIG_FIELD_ELASTICSEARCH = "# ${ELASTICSEARCH}";
    private static final String LOGSTASH_CONFIG_FIELD_ELASTICSEARCH_HOSTS = "${ELASTICSEARCH_HOSTS}";
    private static final int MS_PER_SEC = 1000;

    private static final String OP_REGISTER = "--register";
    private static final String OP_DEREGISTER = "--deregister";
    private static final String OP_DISCOVER = "--discover";
    private static final String OP_LOGSTASH_CONFIG = "--logstash-config";

    /*
     * Miscellaneous constants.
     */
    private static final String STRING_EMPTY = "";
    private static final String STRING_NULL = "null";
    private static final String SEPARATOR_ADDRESS_PORT = ":";
    private static final String DELIMITER_ADDRESS = ",";
    private static final String FILENAME_PROPERTIES = "/opt/adp/conf/service-management-agent.properties";

    private String sdServerAddr;
    private int sdServerPort;
    private String sdServer;
    private ConsulClient consulClient;

    /*
     * Create Consul client using Consul configuration provided in arguments.
     */
    public Client(String sdServerAddr, int sdServerPort) {
        this.sdServerAddr = sdServerAddr;
        this.sdServerPort = sdServerPort;

        if (StringUtils.isEmpty(this.sdServerAddr))
            LOGGER.warn("Undefined SD server address <{}>.", this.sdServerAddr);

        sdServer = sdServerAddr + SEPARATOR_ADDRESS_PORT + sdServerPort;
        LOGGER.info("SD server <{}>.", sdServer);

        if (!StringUtils.isEmpty(this.sdServerAddr))
            consulClient = new ConsulClient(sdServerAddr, sdServerPort);
    }

    /*
     * Create Consul client using Consul configuration provided in environment
     * variables.
     */
    public Client() {
        this(System.getenv(ENV_VAR_SD_SERVER_ADDR), ((!StringUtils
                .isEmpty(System.getenv(ENV_VAR_SD_SERVER_PORT))) ? (Integer
                .parseInt(System.getenv(ENV_VAR_SD_SERVER_PORT)))
                : (DEF_SD_SERVER_PORT)));
    }

    public String getSdServerAddr() {
        return sdServerAddr;
    }

    public int getSdServerPort() {
        return sdServerPort;
    }

    public String getSdServer() {
        return sdServer;
    }

    public ConsulClient getConsulClient() {
        return consulClient;
    }

    /*
     * Register service with SD using service configuration provided in Consul
     * service object.
     */
    public Response<Void> register(NewService service) throws IOException {
        if (service == null)
            throw new IllegalArgumentException("Invalid service <" + service
                    + ">.");

        if (consulClient == null) {
            LOGGER.info("Undefined Consul client <{}>; returning.",
                    consulClient);
            return null;
        }

        Response<Void> response = consulClient.agentServiceRegister(service);
        LOGGER.info(
                "Consul client's agentServiceRegister() called; SD server <{}>, service <{}>, response <{}>.",
                sdServer, service, response);

        propertiesFileStore(service);

        return response;
    }

    /*
     * Register service with SD using service configuration provided in file.
     */
    public Response<Void> register(String filenameServiceConfig)
            throws IOException {
        if (StringUtils.isEmpty(filenameServiceConfig))
            throw new IllegalArgumentException(
                    "Invalid service configuration filename <"
                            + filenameServiceConfig + ">.");

        String content = new String(Files.readAllBytes(Paths
                .get(filenameServiceConfig)));
        NewService service = new Gson().fromJson(content, NewService.class);

        return register(service);
    }

    /*
     * Register service with SD using service configuration provided in
     * environment variables.
     */
    public Response<Void> register() throws IOException {
        NewService service = new NewService();
        service.setCheck(new NewService.Check());

        /*
         * Read service environment variables.
         */
        String value = System.getenv(ENV_VAR_SERVICE_NAME);
        if (!StringUtils.isEmpty(value))
            service.setName(value);

        value = System.getenv(ENV_VAR_INSTANCE_ID);
        if (!StringUtils.isEmpty(value))
            service.setId(value);

        value = System.getenv(ENV_VAR_APP_NAME);
        if (!StringUtils.isEmpty(value))
            service.setTags(Arrays.asList(value));

        value = System.getenv(ENV_VAR_EXT_SERVICE_IP);
        if (!StringUtils.isEmpty(value))
            service.setAddress(value);

        value = System.getenv(ENV_VAR_EXT_SERVICE_PORT);
        if (!StringUtils.isEmpty(value))
            service.setPort(Integer.parseInt(value));

        value = System.getenv(ENV_VAR_SD_HEALTH_CHECK_URL);
        if (!StringUtils.isEmpty(value))
            service.getCheck().setHttp(value);

        value = System.getenv(ENV_VAR_SD_HEALTH_CHECK_INTERVAL);
        if (!StringUtils.isEmpty(value))
            service.getCheck().setInterval(value);

        /*
         * If necessary, set service defaults.
         */
        if (StringUtils.isEmpty(service.getId()))
            service.setId(UUID.randomUUID().toString());

        if ((service.getTags() == null) || (service.getTags().size() < 1))
            service.setTags(Arrays.asList(DEF_APP_NAME));

        if (StringUtils.isEmpty(service.getCheck().getInterval()))
            service.getCheck().setInterval(DEF_SD_HEALTH_CHECK_INTERVAL);

        return register(service);
    }

    /*
     * Deregister service with SD using service ID used during registration.
     */
    public Response<Void> deregister() {
        String serviceId = null;
        try {
            serviceId = propertiesFileLoad().getProperty(ENV_VAR_INSTANCE_ID);
        } catch (IOException e) {
            LOGGER.warn(
                    "Unable to load properties file; exception <{}>; returning.",
                    e.getMessage());
            return null;
        }

        if (StringUtils.isEmpty(serviceId)) {
            LOGGER.info(
                    "Invalid service ID <{}>; unable to deregister; returning.",
                    serviceId);
            return null;
        }

        if (consulClient == null) {
            LOGGER.info("Undefined Consul client <{}>; returning.",
                    consulClient);
            return null;
        }

        Response<Void> response = consulClient
                .agentServiceDeregister(serviceId);
        LOGGER.info(
                "Consul client's agentServiceDeregister() called; SD server <{}>, service ID <{}>, response <{}>.",
                sdServer, serviceId, response);

        return response;
    }

    /*
     * Discover services from SD.
     */
    public Response<List<HealthService>> discover(String serviceName,
            String tag, boolean onlyPassing, QueryParams queryParams) {
        if (consulClient == null) {
            LOGGER.info("Undefined Consul client <{}>; returning.",
                    consulClient);
            return null;
        }

        Response<List<HealthService>> response = consulClient
                .getHealthServices(serviceName, tag, onlyPassing, queryParams);
        LOGGER.info(
                "Consul client's getHealthServices() called; SD server <{}>, serviceName <{}>, tag <{}>, onlyPassing <{}>, queryParams <{}>, response <{}>.",
                sdServer, serviceName, tag, onlyPassing,
                queryParams.toUrlParameters(), response);

        return response;
    }

    /*
     * Get healthy service addresses from Consul. If unable to get from Consul
     * or Consul returned nothing, get from optionally provided environment
     * variable.
     */
    public List<String> getHealthyServiceAddresses(String serviceName,
            String tag, String envVar) {
        if (StringUtils.isEmpty(serviceName))
            throw new IllegalArgumentException("Invalid service name <"
                    + serviceName + ">.");

        if (StringUtils.isEmpty(tag))
            throw new IllegalArgumentException("Invalid tag <" + tag + ">.");

        List<String> listAddress = new LinkedList<String>();
        Response<List<HealthService>> response = discover(serviceName, tag,
                true, QueryParams.DEFAULT);

        if (response != null) {
            List<HealthService> listHealthService = response.getValue();

            if (listHealthService != null)
                for (HealthService i : listHealthService) {
                    Service service = i.getService();
                    if (service == null)
                        continue;

                    String address = service.getAddress();
                    if (StringUtils.isEmpty(address))
                        continue;

                    Integer port = service.getPort();
                    if (port == null)
                        continue;

                    listAddress.add(address + SEPARATOR_ADDRESS_PORT + port);
                }
        }

        if (listAddress.isEmpty())
            listAddress = getHealthyServiceAddresses(envVar);

        LOGGER.info(
                "Healthy service addresses query for service name <{}>, tag <{}> returned healthy service addresses list <{}>.",
                serviceName, tag, listAddress);

        return listAddress;
    }

    /*
     * Get list of healthy service addresses from Consul.
     */
    public List<String> getHealthyServiceAddresses(String serviceName,
            String tag) {
        return getHealthyServiceAddresses(serviceName, tag, null);
    }

    /*
     * Get list of healthy service addresses from environment variable.
     */
    public List<String> getHealthyServiceAddresses(String envVar) {
        if (StringUtils.isEmpty(envVar))
            return null;

        String value = System.getenv(envVar);
        if (StringUtils.isEmpty(value))
            return null;

        String[] valueArray = value.split(DELIMITER_ADDRESS);
        for (int i = 0; i < valueArray.length; ++i)
            valueArray[i] = valueArray[i].trim();

        return Arrays.asList(valueArray);
    }

    private void propertiesFileStore(NewService service) throws IOException {
        if (service == null)
            throw new IllegalArgumentException("Invalid service <" + service
                    + ">.");

        OutputStream os = new FileOutputStream(FILENAME_PROPERTIES);
        Properties properties = new Properties();

        properties.setProperty(ENV_VAR_INSTANCE_ID, ((!StringUtils
                .isEmpty(service.getId())) ? (service.getId()) : ("")));
        properties.store(os, null);

        os.close();
    }

    private Properties propertiesFileLoad() throws IOException {
        InputStream is = new FileInputStream(FILENAME_PROPERTIES);
        Properties properties = new Properties();

        properties.load(is);
        is.close();

        return properties;
    }

    private static boolean isSdDefined() {
        return (!StringUtils.isEmpty(System.getenv(ENV_VAR_SD_SERVER_ADDR)));
    }

    public static String getCurrentTime() {
        return new SimpleDateFormat(TIMESTAMP_FORMAT_STDOUT).format(new Date());
    }

    private static void printUsage() {
        System.out.println();
        System.out.println("USAGE: " + SCRIPT_NAME + " " + OP_REGISTER);
        System.out.println("USAGE: " + SCRIPT_NAME + " " + OP_DEREGISTER);
        System.out.println("       " + SCRIPT_NAME + " " + OP_DISCOVER
                + " {service name} {tag} [environment variable]");
        System.out
                .println("       "
                        + SCRIPT_NAME
                        + " "
                        + OP_LOGSTASH_CONFIG
                        + " {service name} {tag} {environment variable} {Logstash config template file} {Logstash config file} [discover interval]");
        System.out.println();
    }

    /*
     * Convert list of strings to string of comma-separated strings.
     */
    private static String listToString(List<String> list) {
        String string = "";

        if (list == null)
            return string;

        for (String i : list)
            string += (((string.isEmpty()) ? (STRING_EMPTY)
                    : (DELIMITER_ADDRESS)) + '\"' + i + '\"');

        return string;
    }

    private static String getHealthyServiceAddresses(String[] args) {
        String serviceName = args[1];
        String tag = args[2];
        String envVar = ((args.length > 3) ? ((!args[3].equals(STRING_NULL)) ? (args[3])
                : (null))
                : (null));

        return listToString(new Client().getHealthyServiceAddresses(
                serviceName, tag, envVar));
    }

    private static int opRegister(String[] args) throws IOException {
        if (args.length != 1) {
            printUsage();
            return RC_FAILURE;
        }

        if (!isSdDefined()) {
            System.out.println();
            System.out.println(getCurrentTime()
                    + " SD is undefined; not performing register.");
            System.out.println();

            return RC_SUCCESS;
        }

        new Client().register();

        return RC_SUCCESS;
    }

    private static int opDeregister(String[] args) {
        if (args.length != 1) {
            printUsage();
            return RC_FAILURE;
        }

        if (!isSdDefined()) {
            System.out.println();
            System.out.println(getCurrentTime()
                    + " SD is undefined; not performing deregister.");
            System.out.println();

            return RC_SUCCESS;
        }

        new Client().deregister();

        return RC_SUCCESS;
    }

    private static int opDiscover(String[] args) {
        if ((args.length < 3) || (args.length > 4)) {
            printUsage();
            return RC_FAILURE;
        }

        String serviceName = args[1];
        String tag = args[2];
        String healthyServiceAddresses = getHealthyServiceAddresses(args);

        System.out.println(getCurrentTime()
                + " Healthy service addresses query for service name <"
                + serviceName + ">, tag <" + tag
                + "> returned healthy service addresses string <"
                + healthyServiceAddresses + ">.");

        return RC_SUCCESS;
    }

    private static int opLogstashConfig(String[] args) {
        if ((args.length < 6) || (args.length > 7)) {
            printUsage();
            return RC_FAILURE;
        }

        Path pathLogstashConfigTemplate = Paths.get(args[4]);
        Path pathLogstashConfig = Paths.get(args[5]);
        int discoverInterval = ((args.length > 6) ? ((!args[6]
                .equals(STRING_NULL)) ? (Integer.parseInt(args[6])) : (0))
                : (0));

        if (Files.notExists(pathLogstashConfigTemplate)) {
            System.out.println(getCurrentTime() + " File <"
                    + pathLogstashConfigTemplate + "> does not exist.");
            return RC_FAILURE;
        }

        String healthyServiceAddressesPrev = STRING_EMPTY;

        while (true) {
            String healthyServiceAddresses = getHealthyServiceAddresses(args);

            if (!healthyServiceAddresses.equals(healthyServiceAddressesPrev)) {
                String stringLogstashConfigTemplate = null;
                try {
                    stringLogstashConfigTemplate = new String(
                            Files.readAllBytes(pathLogstashConfigTemplate));
                } catch (IOException e) {
                    LOGGER.error("Unable to read file <{}>; exception <{}>.",
                            pathLogstashConfigTemplate, e.getMessage());
                    return RC_FAILURE;
                }

                int i = stringLogstashConfigTemplate
                        .indexOf(LOGSTASH_CONFIG_FIELD_ELASTICSEARCH);
                if (i < 0) {
                    LOGGER.error("Unable to find field <{}> in file <{}>.",
                            LOGSTASH_CONFIG_FIELD_ELASTICSEARCH,
                            pathLogstashConfigTemplate);
                    return RC_FAILURE;
                }

                int j = stringLogstashConfigTemplate.indexOf('\n', i);
                if (j < 0) {
                    LOGGER.error(
                            "Unable to find EOL for field <{}> in file <{}>.",
                            LOGSTASH_CONFIG_FIELD_ELASTICSEARCH,
                            pathLogstashConfigTemplate);
                    return RC_FAILURE;
                }

                String pathString = stringLogstashConfigTemplate.substring(
                        (i + LOGSTASH_CONFIG_FIELD_ELASTICSEARCH.length()), j)
                        .trim();
                Path pathLogstashConfigEsTemplate = Paths.get(pathString);

                if (Files.notExists(pathLogstashConfigEsTemplate)) {
                    System.out.println(getCurrentTime() + " File <"
                            + pathLogstashConfigEsTemplate
                            + "> does not exist.");
                    return RC_FAILURE;
                }

                String stringLogstashConfigEsTemplate = null;
                try {
                    stringLogstashConfigEsTemplate = new String(
                            Files.readAllBytes(pathLogstashConfigEsTemplate));
                } catch (IOException e) {
                    LOGGER.error("Unable to read file <{}>; exception <{}>.",
                            pathLogstashConfigEsTemplate, e.getMessage());
                    return RC_FAILURE;
                }

                try {
                    if (Files.exists(pathLogstashConfig))
                        Files.copy(
                                pathLogstashConfig,
                                Paths.get(pathLogstashConfig.toString()
                                        + "."
                                        + new SimpleDateFormat(TIMESTAMP_FORMAT)
                                                .format(new Date())));
                } catch (IOException e) {
                    LOGGER.warn(
                            "Unable to create backup of file <{}>; exception <{}>.",
                            pathLogstashConfig, e.getMessage());
                }

                try {
                    String stringLogstashConfig = stringLogstashConfigTemplate;

                    if (!healthyServiceAddresses.isEmpty())
                        stringLogstashConfig = stringLogstashConfig.replace(
                                stringLogstashConfigTemplate.substring(i,
                                        (j + 1)),
                                stringLogstashConfigEsTemplate).replace(
                                LOGSTASH_CONFIG_FIELD_ELASTICSEARCH_HOSTS,
                                healthyServiceAddresses);

                    Files.write(pathLogstashConfig,
                            stringLogstashConfig.getBytes());
                } catch (IOException e) {
                    LOGGER.error("Unable to write file <{}>; exception <{}>.",
                            pathLogstashConfig, e.getMessage());
                    return RC_FAILURE;
                }

                System.out
                        .println(getCurrentTime()
                                + " Configured Logstash with healthy Elasticsearch addresses <"
                                + healthyServiceAddresses + ">, replacing <"
                                + healthyServiceAddressesPrev + ">.");

                healthyServiceAddressesPrev = healthyServiceAddresses;
            }

            if (discoverInterval < 1)
                break;

            try {
                Thread.sleep(discoverInterval * MS_PER_SEC);
            } catch (InterruptedException e) {
                break;
            }
        }

        return RC_SUCCESS;
    }

    public static void main(String[] args) throws IOException {
        Runtime.getRuntime().addShutdownHook(new Thread() {
            public void run() {
                LOGGER.info("Shutdown hook called.");
            }
        });

        String op = ((args.length > 0) ? (args[0]) : (""));
        int rc = RC_FAILURE;

        if (op.equals(OP_REGISTER))
            rc = opRegister(args);
        else if (op.equals(OP_DEREGISTER))
            rc = opDeregister(args);
        else if (op.equals(OP_DISCOVER))
            rc = opDiscover(args);
        else if (op.equals(OP_LOGSTASH_CONFIG))
            rc = opLogstashConfig(args);
        else
            printUsage();

        System.exit(rc);
    }
}
