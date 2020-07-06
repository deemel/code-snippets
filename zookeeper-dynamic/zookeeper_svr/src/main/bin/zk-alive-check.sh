#!/bin/bash

${rpm.scriptlet.debug}
${cmd.source.zk.mfdd.lib}

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

# ======================================================================
# Constants.
# ======================================================================

NUM_NOT_SERVING_REQUESTS_MAX=5
LOG_ZK_ALIVE_CHECK="$DIR_ZK_LOG/zk-alive-check.log"

# ======================================================================

cmd="$(basename $0)"

if [ ! -f "$FILE_ZK_CONFIG_SH" ]
then
  echo2 "[$cmd] File <$FILE_ZK_CONFIG_SH> does not exist; exiting <$RC_FAILURE>."
  exit $RC_FAILURE
fi

source $FILE_ZK_CONFIG_SH

hostName="$(hostname -s)"

port=$ZK_PORT
if [ "$ZK_USES_PORT_SECURE" = "$STR_TRUE" ]
then
  port=$ZK_PORT_SECURE
fi

output=$($CMD_FLW "$hostName" $port "srvr" 2> /dev/null)
echo "$output" | grep -q "$FLW_RESP_NOT_SERVING_REQUESTS"

rc=$?
if [ $rc -eq $RC_SUCCESS ]
then
  echo "$(date +"%Y%m%d%H%M%S"): $output" >> $LOG_ZK_ALIVE_CHECK

  echo2 "[$cmd] Contents of log <$LOG_ZK_ALIVE_CHECK>:"
  cat $LOG_ZK_ALIVE_CHECK

  count=$(wc -l $LOG_ZK_ALIVE_CHECK | cut -d' ' -f1)
  if [ $count -ge $NUM_NOT_SERVING_REQUESTS_MAX ]
  then
    echo2 "[$cmd] Detected hung ZK."

    echo2 "[$cmd] Getting ZK status before restarting ZK."
    $CMD_ZK_STATUS_SYSTEMD

    echo2 "[$cmd] Restarting ZK."
    $CMD_ZK_RESTART_SYSTEMD

    echo2 "[$cmd] Getting ZK status after restarting ZK."
    $CMD_ZK_STATUS_SYSTEMD

    echo2 "[$cmd] Emptying log <$LOG_ZK_ALIVE_CHECK>."
    > $LOG_ZK_ALIVE_CHECK
  fi
else
  > $LOG_ZK_ALIVE_CHECK
fi

exit $RC_SUCCESS
