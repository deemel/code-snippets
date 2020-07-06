package com.jupiter.mfdd.security.zk;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.common.X509Util;
import org.eclipse.jetty.util.security.Password;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Credentials {
    private static final Logger LOG = LoggerFactory
            .getLogger(Credentials.class);
    private static final String PROP_USER_NAME = "user.name";
    private static final String PROP_VAL_ZOOKEEPER_CLIENT_SECURE = "true";
    private static final String PARAM_USER_NAME = "<" + PROP_USER_NAME + ">";
    private static final String DIR_CRED_ENV_CLIENT = "MFDD_DIR_CRED";
    private static final String DIR_CRED_ENV_SERVER = "ZK_CRED_DIR";
    private static final String DIR_CRED_DEF = "/etc/opt/mfdd/."
            + PARAM_USER_NAME + "/zk";
    private static final String FILE_CRED = "credentials.properties";

    private static final String FILE_TRUSTSTORE = "truststore.jks";
    private static final String[] TRUSTSTORE_PASS_FILES = {
        "truststore.passphrase.enc",
        "truststore.passphrase",
        "passphrase.enc" };

    private static final String FILE_KEYSTORE = "keystore.jks";
    private static final String[] KEYSTORE_PASS_FILES = {
        "keystore.passphrase.enc",
        "keystore.passphrase",
        "passphrase.enc" };

    private static final String STR_COMMENT = "#";
    private static final String KEY_VALUE_DELIMITER = "=";
    private static final int KEY_VALUE_ARRAY_SZ = 2;
    private static final int KEY_VALUE_MAP_SZ = 4;
    private static final String ZK_CLIENT_OPTS_ENV = "OAM_ZK_CLIENT_OPTS";
    private static final String JAVA_OPT_SYS_PROP = "-D";

    public static void setPropertiesClientFromEnv() {
        String zkClientOpts = System.getenv(ZK_CLIENT_OPTS_ENV);
        if ((zkClientOpts == null) || (zkClientOpts.isEmpty())) {
            LOG.warn("Environment variable <{}> not set; returning.", ZK_CLIENT_OPTS_ENV);
            return;
        }

        String[] props = zkClientOpts.split(JAVA_OPT_SYS_PROP);
        if (props.length < 1) {
            LOG.warn("Environment variable <{}> = <{}> contains no system properties; returning.", ZK_CLIENT_OPTS_ENV, zkClientOpts);
            return;
        }

        LOG.debug("Environment variable <{}> = <{}>.", ZK_CLIENT_OPTS_ENV, zkClientOpts);

        for (String prop : props) {
            if (prop.trim().isEmpty())
                continue;

            String[] keyValue = prop.split(KEY_VALUE_DELIMITER, KEY_VALUE_ARRAY_SZ);
            if (keyValue.length != KEY_VALUE_ARRAY_SZ) {
                LOG.warn("Invalid key-value entry <{}>; continuing.", prop);
                continue;
            }

            String key = keyValue[0].trim();
            String value = keyValue[1].trim();

            System.setProperty(key, value);
            LOG.info("Set system property key <{}>, value <{}>.", key, value);
        }
    }

    public static void setPropertiesClient() {
        setProperties(DIR_CRED_ENV_CLIENT);
        System.setProperty(ZooKeeper.SECURE_CLIENT,
                PROP_VAL_ZOOKEEPER_CLIENT_SECURE);
    }

    public static void setPropertiesServer() {
        setProperties(DIR_CRED_ENV_SERVER);
    }

    private static void setProperties(String dirCredEnv) {
        String userName = System.getProperty(PROP_USER_NAME);
        if ((userName == null) || (userName.isEmpty())) {
            LOG.error("Unable to get system property <{}>; returning.", PROP_USER_NAME);
            return;
        }

        String dirCred = System.getenv(dirCredEnv);
        if ((dirCred == null) || (dirCred.isEmpty())) {
            dirCred = DIR_CRED_DEF.replace(PARAM_USER_NAME, userName);
            LOG.warn("Environment variable <{}> not set; using default value <{}>.", dirCredEnv, dirCred);
        }

        Map<String, String> keyValueMap = new HashMap<String, String>();
        keyValueMap.put(X509Util.SSL_TRUSTSTORE_LOCATION, (dirCred + "/" + FILE_TRUSTSTORE));
        keyValueMap.put(X509Util.SSL_KEYSTORE_LOCATION, (dirCred + "/" + FILE_KEYSTORE));

        keyValueMap.put(X509Util.SSL_TRUSTSTORE_PASSWD, getPassphrase(dirCred, TRUSTSTORE_PASS_FILES));
        keyValueMap.put(X509Util.SSL_KEYSTORE_PASSWD, getPassphrase(dirCred, KEYSTORE_PASS_FILES));

        String filename = dirCred + "/" + FILE_CRED;
        try {
            List<String> lines = Files.readAllLines(Paths.get(filename),
                    StandardCharsets.UTF_8);
            LOG.debug("Read file <{}>.", filename);

            if ((lines != null) && (lines.size() > 0))
                for (String line : lines) {
                    if (line.trim().isEmpty())
                        continue;

                    if (line.startsWith(STR_COMMENT))
                        continue;

                    LOG.debug("Read file <{}>, line <{}>.", filename, line);

                    String[] keyValue = line.split(KEY_VALUE_DELIMITER,
                            KEY_VALUE_ARRAY_SZ);
                    if (keyValue.length != KEY_VALUE_ARRAY_SZ) {
                        LOG.warn("Invalid key-value entry <{}>; continuing.", line);
                        continue;
                    }

                    String key = keyValue[0].trim();
                    switch (key) {
                    case X509Util.SSL_TRUSTSTORE_LOCATION:
                    case X509Util.SSL_TRUSTSTORE_PASSWD:
                    case X509Util.SSL_KEYSTORE_LOCATION:
                    case X509Util.SSL_KEYSTORE_PASSWD:
                        String value = keyValue[1].trim();
                        if (value.isEmpty()) {
                            LOG.warn("Empty value associated with key <{}>; continuing.", key);
                            continue;
                        }

                        if (value.startsWith(Password.__OBFUSCATE))
                            value = Password.deobfuscate(value);

                        keyValueMap.put(key, value);
                        break;

                    default:
                        LOG.warn("Invalid key <{}>; continuing.", key);
                        continue;
                    }
                }
        } catch (IOException e) {
            LOG.debug("Unable to read OPTIONAL file <{}>; NOT an issue; continuing.", filename);
        }

        if (keyValueMap.size() != KEY_VALUE_MAP_SZ) {
            LOG.error("Incomplete credentials provided; returning.");
            return;
        }

        for (Map.Entry<String, String> kv : keyValueMap.entrySet()) {
            if (kv.getValue() != null) {
                System.setProperty(kv.getKey(), kv.getValue());
            }
        }
    }

    private static String getPassphrase(String dir, String[] files) {
        String passphrase = null;
        for (String file : files) {
            passphrase = getPassphrase(dir + "/" + file);
            if (passphrase != null) {
                break;
            }
        }
        return passphrase;
    }

    private static String getPassphrase(String filename) {
        try {
            List<String> lines = Files.readAllLines(Paths.get(filename),
                    StandardCharsets.UTF_8);
            LOG.debug("Read file <{}>.", filename);

            if ((lines == null) || (lines.size() <= 0)) {
                LOG.warn("Empty file <{}>; continuing.", filename);
                return null;
            }

            String line = lines.get(0);
            LOG.debug("Read file <{}>, line <{}>.", filename, line);

            if (line.startsWith(Password.__OBFUSCATE))
                line = Password.deobfuscate(line);

            return line;
        } catch (IOException e) {
            LOG.debug("Unable to read OPTIONAL file <{}>; NOT an issue; continuing.", filename);
            return null;
        }
    }

    public static void main(String[] args) {
        if (args.length < 1)
            return;

        for (String i : args) {
            if (i.startsWith(Password.__OBFUSCATE))
                System.out.println("<" + i + "> -> " + "<" + Password.deobfuscate(i) + ">");
            else
                System.out.println("<" + i + "> -> " + "<" + Password.obfuscate(i) + ">");
        }
    }
}
