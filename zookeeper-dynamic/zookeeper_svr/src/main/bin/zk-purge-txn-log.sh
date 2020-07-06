#!/bin/bash

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

STR_TRUE="${str.true}"

USER_ROOT="${user.root}"
USER_ZOOKEEPER="${user.zookeeper}"

DIR_ZK_ROOT="${dir.zk.root}"
DIR_ZK_LIB="${dir.zk.lib}"

FILE_ZK_CONFIG_SH="${file.zk.config.sh}"

# ======================================================================
# Constants.
# ======================================================================

ZK_SNAP_RETAIN_COUNT_DEF=5

USER=$(whoami)
HOSTNAME=$(hostname -s)

export ZOO_LOG4J_PROP="INFO,ROLLINGFILE"
export ZOO_LOG_DIR="/var/log/mfdd"
export ZOO_LOG_FILE="zookeeper-$USER-client-$HOSTNAME.log"
export JMXDISABLE="$STR_TRUE"
export JVMFLAGS="-server \
  -Xms1g -Xmx1g \
  -XX:CompileThreshold=8000 \
  -XX:NewSize=128m -XX:MaxNewSize=128m \
  -XX:+UseParNewGC \
  -XX:SurvivorRatio=32 \
  -XX:MaxTenuringThreshold=1 \
  -XX:+UseConcMarkSweepGC \
  -XX:CMSInitiatingOccupancyFraction=94 \
  -XX:+UseCMSInitiatingOccupancyOnly \
  -XX:+CMSClassUnloadingEnabled \
  -XX:CMSWaitDuration=500 \
  -XX:-TraceClassUnloading"

ZK_ARGS="-Dzookeeper.root.logger=$ZOO_LOG4J_PROP -Dzookeeper.log.dir=$ZOO_LOG_DIR -Dzookeeper.log.file=$ZOO_LOG_FILE"

# ======================================================================
# Main.
# ======================================================================

if [ "$USER" != "$USER_ROOT" ]
then
  echo "User <$USER> is not <$USER_ROOT>; exiting <$RC_FAILURE>."
  exit $RC_FAILURE
fi

if [ ! -f "$FILE_ZK_CONFIG_SH" ]
then
  echo "File <$FILE_ZK_CONFIG_SH> does not exist; exiting <$RC_FAILURE>."
  exit $RC_FAILURE
fi

source $FILE_ZK_CONFIG_SH

cp=""
for i in $DIR_ZK_ROOT/zookeeper-*.jar $DIR_ZK_LIB/log4j-*.jar $DIR_ZK_LIB/slf4j-*.jar conf
do
  if [ -z "$cp" ]
  then
    cp="$i"
    continue
  fi

  cp="$cp:$i"
done

if [ -z "$ZK_SNAP_RETAIN_COUNT" ]
then
  ZK_SNAP_RETAIN_COUNT=$ZK_SNAP_RETAIN_COUNT_DEF
fi

cd $DIR_ZK_ROOT
java $ZK_ARGS -cp $cp $JVMFLAGS org.apache.zookeeper.server.PurgeTxnLog $ZK_DATA_DIR $ZK_DATA_DIR -n $ZK_SNAP_RETAIN_COUNT
cd - > /dev/null 2>&1

exit $?
