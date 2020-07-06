package com.jupiter.mfdd.security.zk;

import org.apache.zookeeper.server.auth.X509AuthenticationProvider;

/*
 * To configure ZooKeeper server to use this MFDD custom provider for
 * authentication, set following properties in /opt/zookeeper/conf/zookeeper-env.sh.
 *
 * zookeeper.authProvider.x509mfdd=com.jupiter.mfdd.security.zk.X509AuthenticationProviderMfdd
 * zookeeper.ssl.authProvider=x509mfdd
 */
public class X509AuthenticationProviderMfdd extends X509AuthenticationProvider {
    private static String SCHEME = "x509mfdd";

    static {
        Credentials.setPropertiesServer();
    }

    public X509AuthenticationProviderMfdd() {
        super();
    }

    public String getScheme() {
        return SCHEME;
    }
}
