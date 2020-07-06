#!/bin/bash

${rpm.scriptlet.debug}
${cmd.source.zk.mfdd.lib}

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

FILENAME_ZOO_CFG_DYNAMIC="${filename.zoo.cfg.dynamic}"

DIR_TUNNELS_BIN="${dir.tunnels.bin}"

LOG_ZOO_CFG_DYNAMIC_PARSE="${log.zoo.cfg.dynamic.parse}"

CMD_TUNNELS_CREATE="${cmd.tunnels.create}"
CMD_TUNNELS_TOOL="${cmd.tunnels.tool}"

# ======================================================================
# Constants.
# ======================================================================

PREFIX_SSH_TUNNEL_SCRIPT="ssh-tunnel"
NUM_LOGS_MIN=10
NUM_LOGS_MAX=60

# ======================================================================

cmd="$(basename $0)"
timestamp="$(date +"%Y%m%d%H%M%S")"
log="$LOG_ZOO_CFG_DYNAMIC_PARSE.$timestamp"

{
  #
  # Remove older logs so disk won't fill up.
  #
  logs=$(find $(dirname $LOG_ZOO_CFG_DYNAMIC_PARSE) -type f -name "$(basename $LOG_ZOO_CFG_DYNAMIC_PARSE).*" | sort)
  count=$(echo "$logs" | wc -l)
  if [ $count -ge $NUM_LOGS_MAX ]
  then
    numLogsRemove=$(($count - $NUM_LOGS_MIN))
    logsRemove=$(echo $logs | cut -d' ' -f1-$numLogsRemove)

    echo2 "[$cmd] Removing <$numLogsRemove> oldest $LOG_ZOO_CFG_DYNAMIC_PARSE.* logs."
    for i in $logsRemove
    do
      rm -fv $i
    done
  fi

  #
  # Source in zookeeperConfiguration.sh to determine whether secure port, SSH
  # tunnels are being used later on.
  #
  if [ ! -f "$FILE_ZK_CONFIG_SH" ]
  then
    echo2 "[$cmd] File <$FILE_ZK_CONFIG_SH> does not exist; exiting <$RC_FAILURE>."
    exit $RC_FAILURE
  fi

  source $FILE_ZK_CONFIG_SH

  #
  # Check for existence of zoo.cfg.
  #
  if [ ! -f "$FILE_ZOO_CFG" ]
  then
    echo2 "[$cmd] File <$FILE_ZOO_CFG> does not exist; exiting <$RC_FAILURE>."
    exit $RC_FAILURE
  fi

  #
  # Determine current version of zoo.cfg.dynamic.
  #
  funcToCall="grep dynamicConfigFile= $FILE_ZOO_CFG"
  fileZooCfgDynamic=$($funcToCall)

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  fileZooCfgDynamic=$(echo "$fileZooCfgDynamic" | cut -d'=' -f2)
  echo2 "[$cmd] Identified current version of $FILENAME_ZOO_CFG_DYNAMIC <$fileZooCfgDynamic>."

  #
  # Check for existence of current version of zoo.cfg.dynamic.
  #
  if [ ! -f "$fileZooCfgDynamic" ]
  then
    echo2 "[$cmd] File <$fileZooCfgDynamic> does not exist; exiting <$RC_FAILURE>."
    exit $RC_FAILURE
  fi

  #
  # Determine current server configuration.
  #
  funcToCall="grep $ZKCLI_SERVER_SEARCH_STRING $fileZooCfgDynamic"
  zkConfig=$($funcToCall)

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  echo2 "[$cmd] According to ZK, current server configuration <\n$zkConfig>."

  #
  # Convert ZK server configuration to ZK connect string.
  #
  zkConnectString=""
  funcToCall="zkConfigToZkConnectString zkConnectString $zkConfig"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  #
  # Update MFDD_ZK_CONNECT environment variable to ZK connect string.
  #
  funcToCall="envVarAdd \"MFDD_ZK_CONNECT\" \"\\\"$zkConnectString\\\"\" \"$FILE_ZOOKEEPER_ENV\""
  eval "$funcToCall"

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  #
  # If ZK is not using secure ports, exit; no more changes needed at this point.
  #
  if [ "$ZK_USES_PORT_SECURE" != "$STR_TRUE" ]
  then
    exit $RC_SUCCESS
  fi

  #
  # If ZK is not using SSH tunnels, exit; no more changes needed at this point.
  #
  if [ "$ZK_USES_SSH_TUNNELS" != "$STR_TRUE" ]
  then
    exit $RC_SUCCESS
  fi

  #
  # Convert ZK server configuration to ZK SSH tunnel peers list.
  #
  zkSshTunnelPeers=""
  funcToCall="zkConfigToZkSshTunnelPeers zkSshTunnelPeers $zkConfig"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  #
  # Update ZK_SSH_TUNNEL_PEERS environment variable to ZK SSH tunnel peers list.
  #
  funcToCall="envVarAdd \"ZK_SSH_TUNNEL_PEERS\" \"\\\"$zkSshTunnelPeers\\\"\" \"$FILE_ZOOKEEPER_ENV\""
  eval "$funcToCall"

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  #
  # Convert ZK server configuration to ZK SSH tunnel leaders list.
  #
  zkSshTunnelLeaders=""
  funcToCall="zkConfigToZkSshTunnelLeaders zkSshTunnelLeaders $zkConfig"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  #
  # Update ZK_SSH_TUNNEL_LEADERS environment variable to ZK SSH tunnel leaders
  # list.
  #
  funcToCall="envVarAdd \"ZK_SSH_TUNNEL_LEADERS\" \"\\\"$zkSshTunnelLeaders\\\"\" \"$FILE_ZOOKEEPER_ENV\""
  eval "$funcToCall"

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  #
  # Source in up-to-date ZK environment variables.  Based on current server
  # configuration, create, destroy, start, stop tunnels for secure
  # server-to-server communications.
  #
  source $FILE_ZOOKEEPER_ENV

  echo2 "[$cmd] Current tunnels (before changes):"
  ls -l $DIR_TUNNELS_BIN/ssh-*

  echo2 "[$cmd] Current tunnels status (before changes):"
  $CMD_TUNNELS_STATUS_SYSVINIT

  #
  # Create individual tunnel scripts based on ZK_SSH_TUNNEL_PEERS,
  # ZK_SSH_TUNNEL_LEADERS environment variables.  Existing scripts will be
  # overwritten with same scripts.
  #
  funcToCall="$CMD_TUNNELS_CREATE"

  echo2 "[$cmd] Running command <$funcToCall>."
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  #
  # Stop SSH tunnels that are no longer used.  Delete corresponding SSH tunnel
  # scripts.
  #
  sshTunnelScripts=$(find $DIR_TUNNELS_BIN -type l -name "$PREFIX_SSH_TUNNEL_SCRIPT.*")
  zkServers=$(for i in $sshTunnelScripts
  do
    basename $i | cut -d'.' -f3
  done | sort -u)
  for i in $zkServers
  do
    echo "$MFDD_ZK_CONNECT" | grep -q "$i"
    if [ $? -eq $RC_SUCCESS ]
    then
      continue
    fi

    echo2 "[$cmd] ZK server <$i> is no longer in ZK connect string <$MFDD_ZK_CONNECT>; stopping & removing associated SSH tunnels."

    sshTunnelScriptsRemove=$(find $DIR_TUNNELS_BIN -type l -name "$PREFIX_SSH_TUNNEL_SCRIPT.*.$i.*")
    for j in $sshTunnelScriptsRemove
    do
      echo -n "Status (before): "
      $j status

      echo -n "Stopping: "
      $j stop

      echo -n "Status (after): "
      $j status

      rm -fv $j
    done
  done

  #
  # Start individual tunnel scripts.  Already running scripts will be detected,
  # skipped.
  #
  funcToCall="$CMD_TUNNELS_START_SYSVINIT"

  echo2 "[$cmd] Running command <$funcToCall>."
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  echo2 "[$cmd] Current tunnels (after changes):"
  ls -l $DIR_TUNNELS_BIN/ssh-*

  echo2 "[$cmd] Current tunnels status (after changes):"
  $CMD_TUNNELS_STATUS_SYSVINIT

  rc=$?
  exit $rc
} 2>&1 | tee $log

#
# Exit return code is return code from 1st command in pipe, i.e., stdout &
# stderr redirection.  More info at
# https://unix.stackexchange.com/questions/186826/parent-script-continues-when-child-exits-with-non-zero-exit-code.
#
rc=${PIPESTATUS[0]}
exit $rc
