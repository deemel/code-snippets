#!/bin/bash

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

RC_SUCCESS=${rc.success}
RC_FAILURE=${rc.failure}

ZOO_CFG_FIELD_SECURE_CLIENT_PORT="${zoo.cfg.field.secure.client.port}"

DIR_ZK_BIN="${dir.zk.bin}"
DIR_ZK_LOG="${dir.zk.log}"
DIR_ZK_MFDD_BIN="${dir.zk.mfdd.bin}"

FILE_ZOO_CFG="${file.zoo.cfg}"

CMD_EXPORT_OAM_ZK_CLIENT_OPTS="${cmd.export.oam.zk.client.opts}"

# ======================================================================

export ZOOBINDIR="$DIR_ZK_BIN"
export ZOO_LOG_DIR="$DIR_ZK_LOG"
export ZOO_LOG4J_PROP="INFO,ROLLINGFILE"

for i in $DIR_ZK_MFDD_BIN $ZOOBINDIR
do
  echo $PATH | grep -q "$i"
  if [ $? -ne $RC_SUCCESS ]
  then
    export PATH="$i:$PATH"
  fi
done

if [ -f "$FILE_ZOO_CFG" ]
then
  export SERVER_JVMFLAGS="$SERVER_JVMFLAGS -Dzookeeper.admin.serverPort=8088"

  grep -q "^$ZOO_CFG_FIELD_SECURE_CLIENT_PORT=" $FILE_ZOO_CFG
  if [ $? -eq $RC_SUCCESS ]
  then
    $CMD_EXPORT_OAM_ZK_CLIENT_OPTS
    export CLIENT_JVMFLAGS="$CLIENT_JVMFLAGS $OAM_ZK_CLIENT_OPTS"
    export SERVER_JVMFLAGS="$SERVER_JVMFLAGS -Dzookeeper.serverCnxnFactory=org.apache.zookeeper.server.NettyServerCnxnFactory"
    export SERVER_JVMFLAGS="$SERVER_JVMFLAGS -Dzookeeper.ssl.authProvider=x509mfdd"
    export SERVER_JVMFLAGS="$SERVER_JVMFLAGS -Dzookeeper.authProvider.x509mfdd=com.jupiter.mfdd.security.zk.X509AuthenticationProviderMfdd"
  fi
fi

# ======================================================================
# MFDD's ZK environment variables.
# ======================================================================

