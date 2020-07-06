package com.jupiter.mfdd.security.zk;

import org.apache.zookeeper.ClientCnxnSocketNetty;

/*
 * To configure ZooKeeper clients to use this MFDD custom provider for
 * client connections, set following property in /opt/zookeeper/conf/zookeeper-env.sh.
 *
 * zookeeper.clientCnxnSocket="com.jupiter.mfdd.security.zk.ClientCnxnSocketNettyMfdd"
 */
public class ClientCnxnSocketNettyMfdd extends ClientCnxnSocketNetty {
    private Boolean setProperties = Boolean.FALSE;

    public ClientCnxnSocketNettyMfdd() {
        synchronized (setProperties) {
            Credentials.setPropertiesClient();
            setProperties = Boolean.TRUE;
        }
    }

}
