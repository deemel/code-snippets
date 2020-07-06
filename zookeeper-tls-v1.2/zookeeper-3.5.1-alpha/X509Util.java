public static final String PROTOCOL_TLS = "TLS";  // dml
public static final String[] ENABLED_PROTOCOLS = { (PROTOCOL_TLS + "v1"), (PROTOCOL_TLS + "v1.2") };  // dml

    public static SSLContext createSSLContext() throws SSLContextException {
        KeyManager[] keyManagers = null;
        TrustManager[] trustManagers = null;

        String keyStoreLocationProp = System.getProperty(SSL_KEYSTORE_LOCATION);
        String keyStorePasswordProp = System.getProperty(SSL_KEYSTORE_PASSWD);

        // There are legal states in some use cases for null KeyManager or TrustManager.
        // But if a user wanna specify one, location and password are required.

        if (keyStoreLocationProp == null && keyStorePasswordProp == null) {
            LOG.warn("keystore not specified for client connection");
        } else {
            if (keyStoreLocationProp == null) {
                throw new SSLContextException("keystore location not specified for client connection");
            }
            if (keyStorePasswordProp == null) {
                throw new SSLContextException("keystore password not specified for client connection");
            }
            try {
                keyManagers = new KeyManager[]{
                        createKeyManager(keyStoreLocationProp, keyStorePasswordProp)};
            } catch (KeyManagerException e) {
                throw new SSLContextException("Failed to create KeyManager", e);
            }
        }

        String trustStoreLocationProp = System.getProperty(SSL_TRUSTSTORE_LOCATION);
        String trustStorePasswordProp = System.getProperty(SSL_TRUSTSTORE_PASSWD);

        if (trustStoreLocationProp == null && trustStorePasswordProp == null) {
            LOG.warn("keystore not specified for client connection");
        } else {
            if (trustStoreLocationProp == null) {
                throw new SSLContextException("keystore location not specified for client connection");
            }
            if (trustStorePasswordProp == null) {
                throw new SSLContextException("keystore password not specified for client connection");
            }
            try {
                trustManagers = new TrustManager[]{
                        createTrustManager(trustStoreLocationProp, trustStorePasswordProp)};
            } catch (TrustManagerException e) {
                throw new SSLContextException("Failed to create KeyManager", e);
            }
        }

        SSLContext sslContext = null;
        try {
            sslContext = SSLContext.getInstance(PROTOCOL_TLS);  // dml
            sslContext.init(keyManagers, trustManagers, null);
        } catch (Exception e) {
            throw new SSLContextException(e);
        }
        return sslContext;
    }
