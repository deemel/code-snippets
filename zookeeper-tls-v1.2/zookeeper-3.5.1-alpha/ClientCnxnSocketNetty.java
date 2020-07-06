        private synchronized void initSSL(ChannelPipeline pipeline) throws SSLContextException {
            if (sslContext == null || sslEngine == null) {
                sslContext = X509Util.createSSLContext();
                sslEngine = sslContext.createSSLEngine();
                sslEngine.setUseClientMode(true);
                sslEngine.setEnabledProtocols(X509Util.ENABLED_PROTOCOLS);  // dml
            }
            pipeline.addLast("ssl", new SslHandler(sslEngine));
            LOG.info("SSL handler added for channel: {}", pipeline.getChannel());
        }
