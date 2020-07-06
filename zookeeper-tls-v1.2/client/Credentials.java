private static final String PROP_HTTPS_PROTOCOLS = "https.protocols";  // dml
private static final String PROP_VAL_HTTPS_PROTOCOLS = "TLSv1.2";  // dml

    public static void setPropertiesClient() {
        setProperties();
        System.setProperty(ZooKeeper.SECURE_CLIENT, PROP_VAL_ZOOKEEPER_CLIENT_SECURE);
        System.setProperty(PROP_HTTPS_PROTOCOLS, PROP_VAL_HTTPS_PROTOCOLS);  // dml
    }
