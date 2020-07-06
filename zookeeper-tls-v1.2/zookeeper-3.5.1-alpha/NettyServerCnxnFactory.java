    private synchronized void initSSL(ChannelPipeline p)
	throws X509Exception, KeyManagementException, NoSuchAlgorithmException, IOException {
        String authProviderProp = System.getProperty(X509Util.SSL_AUTHPROVIDER);
        SSLContext sslContext;
        if (authProviderProp == null) {
            sslContext = X509Util.createSSLContext();
        } else {
            sslContext = SSLContext.getInstance(X509Util.PROTOCOL_TLS);  // dml
            X509AuthenticationProvider authProvider =
                    (X509AuthenticationProvider)ProviderRegistry.getProvider(
                            System.getProperty(X509Util.SSL_AUTHPROVIDER,
                                    "x509"));

            if (authProvider == null)
            {
                LOG.error("Auth provider not found: {}", authProviderProp);
                throw new SSLContextException(
                        "Could not create SSLContext with specified auth provider: " +
                        authProviderProp);
            }

            sslContext.init(new X509KeyManager[] { authProvider.getKeyManager() },
                            new X509TrustManager[] { authProvider.getTrustManager() },
                            null);
        }

        SSLEngine sslEngine = sslContext.createSSLEngine();
        sslEngine.setUseClientMode(false);
        sslEngine.setNeedClientAuth(true);
        sslEngine.setEnabledProtocols(X509Util.ENABLED_PROTOCOLS);  // dml

        p.addLast("ssl", new SslHandler(sslEngine));
        LOG.info("SSL handler added for channel: {}", p.getChannel());
    }
