#!/bin/bash

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

DIR_ZK_LOG="${dir.zk.log}"

FILE_ZOOKEEPER_CONF="${file.zookeeper.conf}"
FILE_ZOOKEEPER_ENV="${file.zookeeper-env}"

CMD_ZKCLI="${cmd.zkCli}"

# ======================================================================

source $FILE_ZOOKEEPER_CONF
source $FILE_ZOOKEEPER_ENV

#
# Invoke as: audit_log <message>
#
function audit_log() {
  local message="$@"
  local user="$(whoami)"

  logger -p user.info -t "lruStream-$DIR_ZK_LOG/zk.$user.log" -- $(date +"%Y-%m-%dT%H:%M:%S.%3N%z") [PID:$$] $message

  return $?
}

# ======================================================================
# Main.
# ======================================================================

cmd="$(basename $0)"

audit_log "entered $cmd"

$CMD_ZKCLI -server $MFDD_ZK_CONNECT

audit_log "exited $cmd"

exit $?
