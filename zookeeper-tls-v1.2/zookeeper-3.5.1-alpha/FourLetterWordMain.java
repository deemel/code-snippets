    public static String send4LetterWord(String host, int port, String cmd, boolean secure, int timeout)
            throws IOException, SSLContextException {
        LOG.info("connecting to " + host + " " + port);
        Socket sock;
        InetSocketAddress hostaddress= host != null ? new InetSocketAddress(host, port) :
            new InetSocketAddress(InetAddress.getByName(null), port);
        if (secure) {
            LOG.info("using secure socket");
            SSLContext sslContext = X509Util.createSSLContext();
            SSLSocketFactory socketFactory = sslContext.getSocketFactory();
            SSLSocket sslSock = (SSLSocket) socketFactory.createSocket();
            sslSock.setEnabledProtocols(X509Util.ENABLED_PROTOCOLS);  // dml
            sslSock.connect(hostaddress, timeout);
            sslSock.startHandshake();
            sock = sslSock;
        } else {
            sock = new Socket();
            sock.connect(hostaddress, timeout);
        }
        sock.setSoTimeout(timeout);
        BufferedReader reader = null;
        try {
            OutputStream outstream = sock.getOutputStream();
            outstream.write(cmd.getBytes());
            outstream.flush();

            // this replicates NC - close the output stream before reading
            if (!secure) {
                // SSL prohibits unilateral half-close
                sock.shutdownOutput();
            }

            reader =
                    new BufferedReader(
                            new InputStreamReader(sock.getInputStream()));
            StringBuilder sb = new StringBuilder();
            String line;
            while((line = reader.readLine()) != null) {
                sb.append(line + "\n");
            }
            return sb.toString();
        } catch (SocketTimeoutException e) {
            throw new IOException("Exception while executing four letter word: " + cmd, e);
        } finally {
            sock.close();
            if (reader != null) {
                reader.close();
            }
        }
    }
