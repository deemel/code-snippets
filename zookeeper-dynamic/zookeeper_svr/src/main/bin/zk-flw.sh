#!/bin/bash

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

RC_SUCCESS=${rc.success}
STR_TRUE="${str.true}"
STR_FALSE="${str.false}"
COMM_METHOD_NC="${comm.method.nc}"
ZK_RPM_NAME="${zk.rpm.name}"

DIR_ZK_MFDD_BIN="${dir.zk.mfdd.bin}"
DIR_MFDD_ROOT="${dir.mfdd.root}"
DIR_MFDD_CONF="${dir.mfdd.conf}"
DIR_MFDD_LIB="${dir.mfdd.lib}"
DIR_MFDD_LIB_3PP="${dir.mfdd.lib.3pp}"

FILE_ZK_CONFIG_DAT="${file.zk.config.dat}"
FILE_ZOOKEEPER_CONF="${file.zookeeper.conf}"
FILE_ZOOKEEPER_ENV="${file.zookeeper-env}"

# ======================================================================
# Constants.
# ======================================================================

#
# Determine whether ZK is installed.
#
isZkInstalled="$STR_FALSE"

rpm -qa | grep -q "$ZK_RPM_NAME"
if [ $? -eq $RC_SUCCESS ]
then
  isZkInstalled="$STR_TRUE"
fi

MIN_ARGS=3
MAX_ARGS=4

CMD_NC="ncat"
CMD_NC_LINK="nc"
CMD_NC_PY="$DIR_ZK_MFDD_BIN/netcat.py"

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
CLASS_FLW="org.apache.zookeeper.client.FourLetterWordMain"
CLASS_FLW_SECURE="com.jupiter.mfdd.security.zk.FourLetterWordMainMfdd"

source $FILE_ZOOKEEPER_CONF

if [ "$isZkInstalled" = "$STR_TRUE" ]
then
  source $FILE_ZOOKEEPER_ENV
  source $ZOOBINDIR/zkEnv.sh > /dev/null 2>&1
else
  JVMFLAGS=""
  CLASSPATH="$DIR_MFDD_CONF/zk/*:$DIR_MFDD_LIB/zookeeper-security.jar:$DIR_MFDD_LIB_3PP/jetty-util.jar:$DIR_MFDD_LIB_3PP/log4j.jar:$DIR_MFDD_LIB_3PP/slf4j-api.jar:$DIR_MFDD_LIB_3PP/zookeeper.jar"
fi

# ======================================================================

function displayUsage() {
  local func="${FUNCNAME[0]}"
  local cmd="$(basename $0)"

  echo "USAGE: $cmd {server} {port} {flw} [$COMM_METHOD_NC]"

  exit $RC_FAILURE
}

# ======================================================================
# Main.
# ======================================================================

if [ $# -lt $MIN_ARGS ]
then
  displayUsage
fi

if [ $# -gt $MAX_ARGS ]
then
  displayUsage
fi

server="$1"
port=$2
cmd="$3"
commMethod="$4"

if [ "$commMethod" = "$COMM_METHOD_NC" ]
then
  cmdNc="$CMD_NC_PY"

  #
  # Prefer ncat executable (if installed) over netcat.py.
  #
  type $CMD_NC > /dev/null 2>&1
  if [ $? -eq $RC_SUCCESS ]
  then
    cmdNc="$CMD_NC"
  else
    type $CMD_NC_LINK > /dev/null 2>&1
    if [ $? -eq $RC_SUCCESS ]
    then
      cmdNc="$CMD_NC_LINK"
    fi
  fi

  $cmdNc $server $port << EOF
$cmd
EOF

  exit $?
fi

#
# Use ZK's FourLetterWordMain Java program.
#
if [ -f "$FILE_ZK_CONFIG_DAT" ]
then
  source $FILE_ZK_CONFIG_DAT
fi

ZK_USES_PORT_SECURE=${ZK_USES_PORT_SECURE:-$STR_TRUE}

class="$CLASS_FLW"
if [ "$ZK_USES_PORT_SECURE" = "$STR_TRUE" ]
then
  class="$CLASS_FLW_SECURE"
fi

output=$(java $ZK_ARGS -cp $CLASSPATH $JVMFLAGS $class $server $port $cmd $ZK_USES_PORT_SECURE 2>&1)
rc=$?
echo "$output" | grep -Ev "SLF4J:|log4j:"

exit $rc
