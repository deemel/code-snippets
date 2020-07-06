#!/bin/bash

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

DIR_ZK_ROOT="${dir.zk.root}"
DIR_ZK_LIB="${dir.zk.lib}"

# ======================================================================
# Main.
# ======================================================================

for i in $DIR_ZK_ROOT/zookeeper-*.jar $DIR_ZK_LIB/log4j-*.jar $DIR_ZK_LIB/slf4j-*.jar
do
  cp="$cp:$i"
done

java -cp $cp org.apache.zookeeper.server.LogFormatter "$@"

exit $?
