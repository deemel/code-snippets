#!/bin/bash

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

USER_ROOT="${user.root}"
GROUP_ROOT="${group.root}"
USER_ZOOKEEPER="${user.zookeeper}"
GROUP_ZOOKEEPER="${group.zookeeper}"

PERMS_RW_R_R=${perms.rw-.r--.r--}
PERMS_RWX_RX_RX=${perms.rwx.r-x.r-x}

RC_SUCCESS=${rc.success}
RC_FAILURE=${rc.failure}
STR_TRUE="${str.true}"
STR_FALSE="${str.false}"
LOG_DATE_FORMAT="${log.date.format}"
NAME_ZOOKEEPER="${name.zookeeper}"
NAME_TUNNELS="${name.tunnels}"
FLW_RESP_NOT_SERVING_REQUESTS="${flw.resp.not.serving.requests}"

ZOO_CFG_FIELD_SECURE_CLIENT_PORT="${zoo.cfg.field.secure.client.port}"
ZK_CFG_DYNAMIC_ROLE_PARTICIPANT="${zk.cfg.dynamic.role.participant}"

EXT_RPMSAVE="${ext.rpmsave}"

FILENAME_ZOOKEEPER_SERVICE="${filename.zookeeper.service}"
FILENAME_ZOO_CFG_DYNAMIC_PARSE="${filename.zoo.cfg.dynamic.parse}"

DIR_INIT_D="${dir.init.d}"
DIR_ZK_LOG="${dir.zk.log}"
DIR_ZK_ROOT="${dir.zk.root}"
DIR_ZK_BIN="${dir.zk.bin}"
DIR_ZK_LIB="${dir.zk.lib}"
DIR_ZK_MFDD_BIN="${dir.zk.mfdd.bin}"
DIR_TUNNELS_CERTS="${dir.tunnels.certs}"
DIR_MFDD_3PP="${dir.mfdd.3pp}"
DIR_SYSTEMD_SYSTEM="${dir.systemd.system}"

LINK_ZOOKEEPER="${link.zookeeper}"

FILE_ZK_CONFIG_DAT="${file.zk.config.dat}"
FILE_ZK_CONFIG_SH="${file.zk.config.sh}"
FILE_ZK_PURGE="${file.zk.purge}"
FILE_ZK_PURGE_TEMPLATE="${file.zk.purge.template}"
FILE_ZOO_CFG_TEMPLATE="${file.zoo.cfg.template}"
FILE_ZOO_CFG="${file.zoo.cfg}"
FILE_ZOO_CFG_DYNAMIC="${file.zoo.cfg.dynamic}"
FILE_ZOOKEEPER_CONF="${file.zookeeper.conf}"
FILE_ZOOKEEPER_ENV="${file.zookeeper-env}"
FILE_ZOOKEEPER_SERVICE_TEMPLATE="${file.zookeeper.service.template}"
FILE_ZOOKEEPER_SERVICE_SYSTEMD="${file.zookeeper.service.systemd}"

CMD_ZOOKEEPER="${cmd.zookeeper}"
CMD_ZKCLI="${cmd.zkCli}"
CMD_ZKCLI_QUIT="${cmd.zkCli.quit}"
CMD_ZKSERVER_INITIALIZE="${cmd.zkServer-initialize}"
CMD_ZK_ENABLE_SYSVINIT="${cmd.zk.enable.sysvinit}"
CMD_ZK_DISABLE_SYSVINIT="${cmd.zk.disable.sysvinit}"
CMD_ZK_START_SYSVINIT="${cmd.zk.start.sysvinit}"
CMD_ZK_STOP_SYSVINIT="${cmd.zk.stop.sysvinit}"
CMD_ZK_STATUS_SYSVINIT="${cmd.zk.status.sysvinit}"
CMD_ZK_RESTART_SYSVINIT="${cmd.zk.restart.sysvinit}"
CMD_ZK_ENABLE_SYSTEMD="${cmd.zk.enable.systemd}"
CMD_ZK_DISABLE_SYSTEMD="${cmd.zk.disable.systemd}"
CMD_ZK_START_SYSTEMD="${cmd.zk.start.systemd}"
CMD_ZK_STOP_SYSTEMD="${cmd.zk.stop.systemd}"
CMD_ZK_STATUS_SYSTEMD="${cmd.zk.status.systemd}"
CMD_ZK_RESTART_SYSTEMD="${cmd.zk.restart.systemd}"
CMD_TUNNELS_ENABLE_SYSVINIT="${cmd.tunnels.enable.sysvinit}"
CMD_TUNNELS_DISABLE_SYSVINIT="${cmd.tunnels.disable.sysvinit}"
CMD_TUNNELS_START_SYSVINIT="${cmd.tunnels.start.sysvinit}"
CMD_TUNNELS_STOP_SYSVINIT="${cmd.tunnels.stop.sysvinit}"
CMD_TUNNELS_STATUS_SYSVINIT="${cmd.tunnels.status.sysvinit}"
CMD_FLW="${cmd.flw}"
CMD_ZOO_CFG_DYNAMIC_PARSE="${cmd.zoo.cfg.dynamic.parse}"

# ======================================================================
# Constants.
# ======================================================================

FILE_SBIN_INIT="/sbin/init"
INIT_PROCESS_SYSVINIT="SysVinit"
INIT_PROCESS_UPSTART="upstart"
INIT_PROCESS_SYSTEMD="systemd"

CMD_ZKCLI_CONFIG="config"
CMD_ZKCLI_RECONFIG="reconfig"

ZK_RECONFIG_OP_ADD="-add"
ZK_RECONFIG_OP_REMOVE="-remove"
ZK_RECONFIG_OP_MEMBERS="-members"
ZK_RECONFIG_OP_FILE="-file"

FLW_CMD_STAT="stat"
FLW_CMD_CONF="conf"
FLW_CMD_RUOK="ruok"
FLW_RESP_IMOK="imok"

CLIENT_PORT_ADDRESS="$(hostname -s)"
CLIENT_PORT_ADDRESS_TUNNEL="localhost"

ZK_PORT_DEF=2181
ZK_PORT_SECURE_DEF=2281
ZK_PORT_PEER_DEF=2888
ZK_PORT_LEADER_DEF=3888
ZK_PORT_MULTIPLIER=10
ZK_PORT_PEER_OFFSET=0
ZK_PORT_LEADER_OFFSET=-2000
ZK_USES_PORT_SECURE_DEF="$STR_TRUE"
ZK_USES_SSH_TUNNELS_DEF="$STR_TRUE"
ZK_MYID_INITIAL=0
ZK_DATA_DIR_DEF="/cvar/var/oam/zookeeper"
ZK_RPM_NO_SCRIPTS_DEF="$STR_TRUE"
MFDD_DIR_CRED_DEF="/$USER_ROOT/.ssl/lru-cfg-zk/client"
ZK_CRED_DIR_DEF="/home/$USER_ZOOKEEPER/.ssl/lru-cfg-zk/server"

EXT_DEF="_DEF"
CMD_ZOO_CFG_DYNAMIC_PARSE_CRONTAB_FREQ=1 # minute
CMD_ZOO_CFG_DYNAMIC_PARSE_TIMEOUT="${CMD_ZOO_CFG_DYNAMIC_PARSE_CRONTAB_FREQ}m"
ZKCLI_TRY_MAX=10
ZKCLI_SERVER_SEARCH_STRING="^server\\."
ZKCLI_OUTPUT_BAD_ARGUMENTS="KeeperErrorCode = BadArguments"
ZK_PENDING_SYNCS_TRY_MAX=10
FILENAME_MYID="myid"
FILE_TUNNELS_CERTS="/opt/mfdd/tmp/SSH_DST/CA.tar"

# ======================================================================
# Utility functions.
# ======================================================================

#
# Skip a line before echoing text to stdout.
#
function echo2() {
  local func="${FUNCNAME[0]}"
  local text="$@"
  local timestamp=$(date +"$LOG_DATE_FORMAT")
  local rc=""

  echo
  echo -e "$timestamp $text"

  rc=$?
  return $rc
}

#
# Call function or command.  If non-0 (i.e., failure) return code is returned
# & script is being run by RPM package manager, script must EXIT with 0 (i.e.,
# success) exit code.  More info at
# https://fedoraproject.org/wiki/Packaging:Scriptlets.  Otherwise, script will
# exit with non-0 return code.
#
function call() {
  local func="${FUNCNAME[0]}"
  local rc=""

  local funcToCall="$@"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    if [ "$IS_RPM_RUNNING" = "$STR_TRUE" ]
    then
      rc=$RC_SUCCESS
    fi

    echo2 "Call <$funcToCall> failed with return code <$rc>; exiting <$rc>."
    exit $rc
  fi

  return $rc
}

#
# Add or update exported environment variable in file.
#
function envVarAdd() {
  local func="${FUNCNAME[0]}"
  local var="$1"
  local value="$2"
  local file="$3"
  local cmdExportLhs="export $var"
  local cmdExport="$cmdExportLhs=$value"
  local rc=$RC_FAILURE

  if [ ! -f "$file" ]
  then
    echo2 "[$func] File <$file> does not exist; returning <$rc>."
    return $rc
  fi

  local output=$(grep "$var" $file)
  if [ -z "$output" ]
  then
    echo2 "[$func] Adding environment variable <$var> with value <$value> in file <$file>."

    #
    # Add new environment variable.
    #
    cat >> $file << EOF
$cmdExport
EOF
  else
    if [ "$output" = "$cmdExport" ]
    then
      echo2 "[$func] Environment variable <$output> already set to value <$value> in file <$file>; continuing."
    else
      echo2 "[$func] Updating environment variable <$output> with value <$value> in file <$file>."

      #
      # Update existing environment variable.
      #
      # Escape path for sed using bash find and replace.  For more info, refer
      # to http://www.cyberciti.biz/faq/unix-linux-replace-string-words-in-many-files.
      #
      strSearch="$output"
      strSearch="${strSearch//\//\\/}"
      strReplace="$cmdExport"
      strReplace="${strReplace//\//\\/}"
      sed -i "s/$strSearch/$strReplace/g" $file
    fi
  fi

  echo2 "[$func] Searching for environment variable <$var> in file <$file>."
  grep -H "$cmdExportLhs" "$file"

  rc=$?
  return $rc
}

#
# Get init process.  SysVinit, Upstart, & systemd supported.
#
function initProcessGet() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local _initProcess=""
  local rc=""

  local funcToCall="rpm -qf $FILE_SBIN_INIT"
  local output=$($funcToCall)

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  echo2 "[$func] File <$FILE_SBIN_INIT> is owned by RPM package <$output>."

  #
  # Note that Upstart is backward compatibile with SysVinit.
  #
  for i in "$INIT_PROCESS_SYSVINIT" "$INIT_PROCESS_UPSTART" "$INIT_PROCESS_SYSTEMD"
  do
    echo "$output" | grep -q "$i"

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      continue
    fi

    _initProcess="$i"
    break
  done

  if [ -z "$_initProcess" ]
  then
    rc=$RC_FAILURE
    echo2 "[$func] Unable to determine init process from RPM package <$output>; returning <$rc>."
  else
    rc=$RC_SUCCESS
    echo2 "[$func] Init process <$_initProcess> is assumed; returning <$rc>."
  fi

  eval "$_result=\"$_initProcess\""
  return $rc
}

# ======================================================================
# Tunnels SysVinit service functions.
# ======================================================================

#
# Install tunnels init script for SysVinit.  Tunnels init script link was
# created during ZK RPM installation.  Function included here for completeness.
#
function tunnelsInitScriptInstall() {
  local func=${FUNCNAME[0]}
  local rc=""

  return $RC_SUCCESS
}

#
# Uninstall tunnels init scripts for SysVinit.
#
function tunnelsInitScriptUninstall() {
  local func=${FUNCNAME[0]}
  local rc=""

  echo2 "[$func]"
  find /etc/rc*.d -type l -name "*$NAME_TUNNELS" | xargs rm -fv

  rc=$?
  return $rc
}

#
# Enable tunnels service via SysVinit.
#
function tunnelsEnable() {
  local func="${FUNCNAME[0]}"
  local rc=""

  echo2 "[$func]"
  $CMD_TUNNELS_ENABLE_SYSVINIT

  rc=$?
  return $rc
}

#
# Disable tunnels service via SysVinit.
#
function tunnelsDisable() {
  local func="${FUNCNAME[0]}"
  local rc=""

  echo2 "[$func]"
  $CMD_TUNNELS_DISABLE_SYSVINIT

  rc=$?
  return $rc
}

#
# Start tunnels service via SysVinit.
#
function tunnelsStart() {
  local func="${FUNCNAME[0]}"
  local rc=""

  echo2 "[$func]"
  $CMD_TUNNELS_START_SYSVINIT

  rc=$?
  return $rc
}

#
# Stop tunnels service via SysVinit.
#
function tunnelsStop() {
  local func="${FUNCNAME[0]}"
  local rc=""

  echo2 "[$func]"
  $CMD_TUNNELS_STOP_SYSVINIT

  rc=$?
  return $rc
}

#
# Get status of tunnels service via SysVinit.
#
function tunnelsStatus() {
  local func="${FUNCNAME[0]}"
  local rc=""

  echo2 "[$func]"
  $CMD_TUNNELS_STATUS_SYSVINIT

  rc=$?
  return $rc
}

#
# Deploy tunnels-related keys, certificates.
#
function tunnelsCertificatesDeploy() {
  local func="${FUNCNAME[0]}"
  local funcToCall=""
  local rc=""

  if [ ! -f "$FILE_TUNNELS_CERTS" ]
  then
    echo2 "[$func] File <$FILE_TUNNELS_CERTS> does not exist; not [re]deploying tunnels-related keys & certificates; returning <$RC_SUCCESS>."
    return $RC_SUCCESS
  fi

  funcToCall="$DIR_TUNNELS_CERTS/setup_zk_tunnel_keys.sh"

  echo2 "[$func] Running command <$funcToCall>."
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  funcToCall="$DIR_TUNNELS_CERTS/post_certs_cleanup.sh"

  echo2 "[$func] Running command <$funcToCall>."
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
  fi

  return $rc
}

# ======================================================================
# ZK SysVinit service functions.
# ======================================================================

#
# Install ZK init script for SysVinit.
#
function zkInitScriptInstall() {
  local func=${FUNCNAME[0]}
  local rc=""

  #
  # Create MFDD's ZK init script link in /etc/init.d, which in turn is link to
  # /etc/rc.d/init.d.
  #
  echo2 "[$func]"
  ln -sfv $CMD_ZOOKEEPER $DIR_INIT_D

  rc=$?
  return $rc
}

#
# Uninstall ZK init scripts for SysVinit.
#
function zkInitScriptUninstall() {
  local func=${FUNCNAME[0]}
  local rc=""

  echo2 "[$func]"
  find /etc/rc*.d -name "*$NAME_ZOOKEEPER" -type l | xargs rm -fv

  rc=$?
  return $rc
}

#
# Enable ZK service via SysVinit.
#
function zkEnableSysVinit() {
  local func="${FUNCNAME[0]}"
  local rc=""

  echo2 "[$func]"
  $CMD_ZK_ENABLE_SYSVINIT

  rc=$?
  return $rc
}

#
# Disable ZK service via SysVinit.
#
function zkDisableSysVinit() {
  local func="${FUNCNAME[0]}"
  local rc=""

  echo2 "[$func]"
  $CMD_ZK_DISABLE_SYSVINIT

  rc=$?
  return $rc
}

#
# Set up ZK service for SysVinit.
#
function zkServiceSetUpSysVinit() {
  local func="${FUNCNAME[0]}"
  local isStartOnly="$1"
  local funcToCall=""
  local rc=""

  if [ "$isStartOnly" != "$STR_TRUE" ]
  then
    funcToCall="zkInitScriptInstall"
    $funcToCall

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
      return $rc
    fi

    funcToCall="zkEnableSysVinit"
    $funcToCall

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
      return $rc
    fi
  fi

  echo2 "[$func] Getting ZK status before [re]starting ZK."
  funcToCall="$CMD_ZK_STATUS_SYSVINIT"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; continuing."
  fi

  echo2 "[$func] [Re]starting ZK."
  funcToCall="$CMD_ZK_RESTART_SYSVINIT"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  sleep 3

  echo2 "[$func]"
  funcToCall="$CMD_ZK_STATUS_SYSVINIT"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
  fi

  return $rc
}

#
# Tear down ZK service for SysVinit.
#
function zkServiceTearDownSysVinit() {
  local func="${FUNCNAME[0]}"
  local isStopOnly="$1"
  local rc=""

  if [ ! -f "$FILE_ZOO_CFG" ]
  then
    echo2 "[$func] Unable to stop ZK service because file <$FILE_ZOO_CFG> does not exist; continuing."
  else
    echo2 "[$func]"
    $CMD_ZK_STOP_SYSVINIT
  fi

  echo2 "[$func]"
  $CMD_ZK_STATUS_SYSVINIT

  if [ "$isStopOnly" != "$STR_TRUE" ]
  then
    zkDisableSysVinit
    zkInitScriptUninstall
  fi

  rc=$?
  return $rc
}

# ======================================================================
# ZK systemd service functions.
# ======================================================================

#
# Install ZK unit file for systemd.  Assumes zkConfigDatRead was called
# successfully.
#
function zkUnitFileInstall() {
  local func=${FUNCNAME[0]}
  local stringReplace=""
  local rc=""

  echo2 "[$func]"

  cp -pfv $FILE_ZOOKEEPER_SERVICE_TEMPLATE $FILE_ZOOKEEPER_SERVICE_SYSTEMD
  chown $USER_ROOT:$GROUP_ROOT $FILE_ZOOKEEPER_SERVICE_SYSTEMD
  chmod $PERMS_RW_R_R $FILE_ZOOKEEPER_SERVICE_SYSTEMD

  stringReplace="$ZK_DATA_DIR"
  stringReplace="${stringReplace//\//\\/}"
  eval "sed -i 's/MFDD_dataDir/"$stringReplace"/g' $FILE_ZOOKEEPER_SERVICE_SYSTEMD"

  rc=$?
  return $rc
}

#
# Uninstall ZK unit file for systemd.
#
function zkUnitFileUninstall() {
  local func=${FUNCNAME[0]}
  local rc=""

  echo2 "[$func]"
  rm -fv $DIR_SYSTEMD_SYSTEM/$FILENAME_ZOOKEEPER_SERVICE

  rc=$?
  return $rc
}

#
# Enable ZK service via systemd's systemctl.
#
function zkEnableSystemd() {
  local func="${FUNCNAME[0]}"
  local rc=""

  echo2 "[$func]"
  $CMD_ZK_ENABLE_SYSTEMD

  rc=$?
  return $rc
}

#
# Disable ZK service via systemd's systemctl.
#
function zkDisableSystemd() {
  local func="${FUNCNAME[0]}"
  local rc=""

  echo2 "[$func]"
  $CMD_ZK_DISABLE_SYSTEMD

  rc=$?
  return $rc
}

#
# Set up ZK service for Systemd.
#
function zkServiceSetUpSystemd() {
  local func="${FUNCNAME[0]}"
  local isStartOnly="$1"
  local funcToCall=""
  local rc=""

  if [ "$isStartOnly" != "$STR_TRUE" ]
  then
    funcToCall="zkUnitFileInstall"
    $funcToCall

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
      return $rc
    fi

    #
    # systemd's interaction with SELinux enforcement is supposedly causing
    # systemctl errors such as the following sometimes.
    #   - Failed to execute operation: Access denied
    #   - Failed to start zookeeper.service: Access denied
    # Rather than reduce or disable SELinux enforcement, re-executing systemd
    # daemon is a workaround.  More info at
    # https://major.io/2015/09/18/systemd-in-fedora-22-failed-to-restart-service-access-denied/
    #
    funcToCall="systemctl daemon-reexec"
    $funcToCall

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
      return $rc
    fi

    funcToCall="zkEnableSystemd"
    $funcToCall

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
      return $rc
    fi
  fi

  echo2 "[$func] Getting ZK status before [re]starting ZK."
  funcToCall="$CMD_ZK_STATUS_SYSTEMD"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; continuing."
  fi

  echo2 "[$func] [Re]starting ZK."
  funcToCall="$CMD_ZK_RESTART_SYSTEMD"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  echo2 "[$func] Getting ZK status after [re]starting ZK."
  funcToCall="$CMD_ZK_STATUS_SYSTEMD"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
  fi

  return $rc
}

#
# Tear down ZK service for systemd.
#
function zkServiceTearDownSystemd() {
  local func="${FUNCNAME[0]}"
  local isStopOnly="$1"
  local rc=""

  if [ ! -f "$FILE_ZOO_CFG" ]
  then
    echo2 "[$func] Unable to stop ZK service because file <$FILE_ZOO_CFG> does not exist; continuing."
  else
    echo2 "[$func]"
    $CMD_ZK_STOP_SYSTEMD
  fi

  echo2 "[$func]"
  $CMD_ZK_STATUS_SYSTEMD

  if [ "$isStopOnly" != "$STR_TRUE" ]
  then
    zkDisableSystemd
    zkUnitFileUninstall
  fi

  rc=$?
  return $rc
}

# ======================================================================
# zookeeperConfiguration.dat functions.
# ======================================================================

#
# Convert port number (e.g., ZK peer port, ZK leader port) to local port
# number.  This is for secure ZK server-to-server communications, using local
# port forwarding with SSH tunnel.
#
function portToTunnelPort() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local port=$2
  local offset=$3
  local multiplier=$4
  local index=$5
  local _tunnelPort=$(($multiplier * ($port + $offset) + $index))
  local rc=""

  eval "$_result=$_tunnelPort"

  rc=$?
  return $rc
}

#
# Get ZK CLI port, deriving it from zookeeperConfiguration.dat.  Assumes
# zkConfigDatRead was called successfully.
#
function zkCliPortGet() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local rc=""

  local _zkCliPort=$ZK_PORT
  if [ ! -z "$ZK_PORT_SECURE" ]
  then
    _zkCliPort=$ZK_PORT_SECURE
  fi

  eval "$_result=\"$_zkCliPort\""

  rc=$?
  return $rc
}

#
# Get ZK connect string, deriving it from zookeeperConfiguration.dat.  Assumes
# zkConfigDatRead was called successfully.
#
function zkConnectStringGet() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local zkCliPort=$2
  local rc=""

  local zkServers="${ZK_SERVERS//,/$' '}"
  local _zkConnectString=""
  for i in $zkServers
  do
    local zkServer="$i:$zkCliPort"

    if [ -z "$_zkConnectString" ]
    then
      _zkConnectString="$zkServer"
      continue
    fi

    _zkConnectString="$_zkConnectString,$zkServer"
  done

  eval "$_result=\"$_zkConnectString\""

  rc=$?
  return $rc
}

#
# Get this ZK's myid.  Assumes zkConfigDatRead was called successfully.
#
function zkMyidGet() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local rc=$RC_FAILURE

  local zkServers="${ZK_SERVERS//,/$' '}"
  local _zkMyid=$ZK_MYID_INITIAL
  for i in $zkServers
  do
    if [ "$i" != "$CLIENT_PORT_ADDRESS" ]
    then
      _zkMyid=$(($_zkMyid + 1))
      continue
    fi

    eval "$_result=\"$_zkMyid\""

    rc=$?
    return $rc
  done

  echo2 "[$func] ZK server <$CLIENT_PORT_ADDRESS> does not exist in ZK servers <$zkServers>; should have been defined in file <$FILE_ZK_CONFIG_DAT>; returning <$rc>."
  return $rc
}

#
# Create zookeeperConfiguration.sh.
#
function zkConfigShCreate() {
  local func="${FUNCNAME[0]}"
  local funcToCall=""
  local rc=""

  #
  # Get ZK CLI port.
  #
  local zkCliPort=""
  funcToCall="zkCliPortGet zkCliPort"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  #
  # Get ZK connect string.
  #
  local zkConnectString=""
  funcToCall="zkConnectStringGet zkConnectString $zkCliPort"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  #
  # Get this ZK's myid.
  #
  local zkMyid=""
  funcToCall="zkMyidGet zkMyid"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  #
  # If necessary, get this ZK's local peer & leader ports for SSH tunnel
  # communications.
  #
  zkPortPeerTunnel=""
  zkPortLeaderTunnel=""

  if [ "$ZK_USES_PORT_SECURE" = "$STR_TRUE" ]
  then
    if [ "$ZK_USES_SSH_TUNNELS" = "$STR_TRUE" ]
    then
      funcToCall="portToTunnelPort zkPortPeerTunnel $ZK_PORT_PEER $ZK_PORT_PEER_OFFSET $ZK_PORT_MULTIPLIER $zkMyid"
      $funcToCall

      rc=$?
      if [ $rc -ne $RC_SUCCESS ]
      then
        echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
        return $rc
      fi

      funcToCall="portToTunnelPort zkPortLeaderTunnel $ZK_PORT_LEADER $ZK_PORT_LEADER_OFFSET $ZK_PORT_MULTIPLIER $zkMyid"
      $funcToCall

      rc=$?
      if [ $rc -ne $RC_SUCCESS ]
      then
        echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
        return $rc
      fi
    fi
  fi

  #
  # Create ZK configuration source script.
  #
  for i in "ZK_PORT" "ZK_PORT_SECURE" "ZK_PORT_PEER" "ZK_PORT_LEADER" "ZK_SERVERS" "ZK_USES_PORT_SECURE" "ZK_USES_SSH_TUNNELS" "ZK_LOCAL_PORTS_PEER" "ZK_LOCAL_PORTS_LEADER" "ZK_DATA_DIR"
  do
    echo "$i=${!i}"
  done > $FILE_ZK_CONFIG_SH

  cat >> $FILE_ZK_CONFIG_SH << EOF
ZK_FILE_MYID=$ZK_DATA_DIR/$FILENAME_MYID
ZK_CLI_PORT=$zkCliPort
ZK_CONNECT_STRING="$zkConnectString"
ZK_MYID=$zkMyid
ZK_PORT_PEER_TUNNEL=$zkPortPeerTunnel
ZK_PORT_LEADER_TUNNEL=$zkPortLeaderTunnel
EOF

  chown $USER_ZOOKEEPER:$GROUP_ZOOKEEPER $FILE_ZK_CONFIG_SH
  chmod $PERMS_RW_R_R $FILE_ZK_CONFIG_SH

  echo2 "[$func] Contents of file <$FILE_ZK_CONFIG_SH>:"
  cat $FILE_ZK_CONFIG_SH

  source $FILE_ZK_CONFIG_SH

  rc=$?
  return $rc
}

#
# Create zkPurge.conf.  Assumes zkConfigShCreate was called successfully.
#
function zkPurgeConfCreate() {
  local func=${FUNCNAME[0]}
  local stringReplace=""
  local rc=""

  echo2 "[$func]"

  cp -pfv $FILE_ZK_PURGE_TEMPLATE $FILE_ZK_PURGE
  chown $USER_ZOOKEEPER:$GROUP_ZOOKEEPER $FILE_ZK_PURGE
  chmod $PERMS_RW_R_R $FILE_ZK_PURGE

  stringReplace="$ZK_DATA_DIR"
  stringReplace="${stringReplace//\//\\/}"
  eval "sed -i 's/MFDD_dataDir/"$stringReplace"/g' $FILE_ZK_PURGE"

  echo2 "[$func] Contents of file <$FILE_ZK_PURGE>:"
  cat $FILE_ZK_PURGE

  rc=$?
  return $rc
}

#
# Read & validate zookeeperConfiguration.dat.  Create
# zookeeperConfiguration.sh, containing useful ZK-related environment variables
# derived from zookeeperConfiguration.dat.
#
function zkConfigDatRead() {
  local func="${FUNCNAME[0]}"
  local funcToCall=""
  local rc=$RC_FAILURE

  #
  # Read zookeeper.conf for MFDD_DIR_CRED environment variable, representing
  # location of ZK client credentials.
  #
  if [ ! -f "$FILE_ZOOKEEPER_CONF" ]
  then
    echo2 "[$func] File <$FILE_ZOOKEEPER_CONF> does not exist; continuing."
  else
    echo2 "[$func] Contents of file <$FILE_ZOOKEEPER_CONF>:"
    cat $FILE_ZOOKEEPER_CONF

    source $FILE_ZOOKEEPER_CONF
  fi

  if [ ! -f "$FILE_ZK_CONFIG_DAT" ]
  then
    echo2 "[$func] File <$FILE_ZK_CONFIG_DAT> does not exist; returning <$rc>."
    return $rc
  fi

  echo2 "[$func] Contents of file <$FILE_ZK_CONFIG_DAT>:"
  cat $FILE_ZK_CONFIG_DAT

  source $FILE_ZK_CONFIG_DAT

  for i in "ZK_SERVERS"
  do
    #
    # Note indirect variable reference (variable dereference).
    #
    if [ -z ${!i} ]
    then
      echo2 "[$func] Environment variable <$i> does not exist; should have been defined in file <$FILE_ZK_CONFIG_DAT>; returning <$rc>."
      return $rc
    fi
  done

  for i in "ZK_PORT" "ZK_PORT_SECURE" "ZK_PORT_PEER" "ZK_PORT_LEADER" "ZK_USES_PORT_SECURE" "ZK_USES_SSH_TUNNELS" "ZK_DATA_DIR" "MFDD_DIR_CRED" "ZK_CRED_DIR"
  do
    if [ -z ${!i} ]
    then
      local varDef="$i$EXT_DEF"
      local valDef=${!varDef}

      echo2 "[$func] Environment variable <$i> does not exist; defaulting to value <$valDef>."

      eval "$i=$valDef"
    fi
  done

  if [ "$ZK_USES_PORT_SECURE" = "$STR_TRUE" ]
  then
    if [ "$ZK_USES_SSH_TUNNELS" != "$STR_TRUE" ]
    then
      ZK_USES_SSH_TUNNELS="$STR_FALSE"
    fi
  else
    ZK_USES_PORT_SECURE="$STR_FALSE"
    ZK_USES_SSH_TUNNELS="$STR_FALSE"

    for i in "ZK_PORT_SECURE" "ZK_LOCAL_PORTS_PEER" "ZK_LOCAL_PORTS_LEADER"
    do
      eval "$i=\"\""
    done
  fi

  echo2 "[$func] ZK configuration with optional parameters set to default values:"
  for i in "ZK_PORT" "ZK_PORT_SECURE" "ZK_PORT_PEER" "ZK_PORT_LEADER" "ZK_SERVERS" "ZK_USES_PORT_SECURE" "ZK_USES_SSH_TUNNELS" "ZK_LOCAL_PORTS_PEER" "ZK_LOCAL_PORTS_LEADER" "ZK_DATA_DIR" "MFDD_DIR_CRED" "ZK_CRED_DIR"
  do
    echo "$i=${!i}"
  done

  funcToCall="zkConfigShCreate"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
  fi

  funcToCall="zkPurgeConfCreate"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
  fi

  return $rc
}

#
# Add ZK environment variables to zookeeper-env.sh.  Assumes zkConfigDatRead
# was called successfully.
#
function zkEnvVarsAdd() {
  local func="${FUNCNAME[0]}"
  local funcToCall=""
  local rc=""

  funcToCall="envVarAdd \"ZK_THIS_SERVER\" \"\\\"$CLIENT_PORT_ADDRESS\\\"\" \"$FILE_ZOOKEEPER_ENV\""
  eval "$funcToCall"

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  funcToCall="envVarAdd \"MFDD_ZK_CONNECT\" \"\\\"$ZK_CONNECT_STRING\\\"\" \"$FILE_ZOOKEEPER_ENV\""
  eval "$funcToCall"

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
  fi

  if [ ! -z "$ZK_CRED_DIR" ]
  then
    echo2 "[$func] adding optional ZK_CRED_DIR=$ZK_CRED_DIR to $FILE_ZOOKEEPER_ENV"

    funcToCall="envVarAdd \"ZK_CRED_DIR\" \"\\\"$ZK_CRED_DIR\\\"\" \"$FILE_ZOOKEEPER_ENV\""
    eval "$funcToCall"

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
      return $rc
    fi
  fi

  return $rc
}

# ======================================================================
# ZK CLI functions.
# ======================================================================

#
# Get accessible ZK server to use with ZK CLI.  List of candidates comes from
# (in order of preference) environment variable MFDD_ZK_CONNECT value, value
# from zookeeperConfiguation.sh (assuming zkConfigDatRead was called
# successfully).  From list of candidates, select ZK server that is accessible.
#
function zkCliConnectStringGet() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local zkConnectString=""
  local rc=$RC_FAILURE

  #
  # Check for ZK connect string provided by MFDD_ZK_CONNECT environment
  # variable.
  #
  if [ -f "$FILE_ZOOKEEPER_ENV" ]
  then
    source $FILE_ZOOKEEPER_ENV

    if [ ! -z "$MFDD_ZK_CONNECT" ]
    then
      zkConnectString="$MFDD_ZK_CONNECT"
    fi
  fi

  if [ -z "$zkConnectString" ]
  then
    #
    # Check for ZK connect string provided by ZK_CONNECT_STRING environment
    # variable.
    #
    if [ -f "$FILE_ZK_CONFIG_SH" ]
    then
      source $FILE_ZK_CONFIG_SH

      if [ ! -z "$ZK_CONNECT_STRING" ]
      then
        zkConnectString="$ZK_CONNECT_STRING"
      fi
    fi
  fi

  if [ -z "$zkConnectString" ]
  then
    echo2 "[$func] Unable to get ZK connect string; returning <$rc>."
    return $rc
  fi

  local zkConnectStringOk=""
  local funcToCall="whoIsOk zkConnectStringOk $zkConnectString"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  eval "$_result=\"$zkConnectStringOk\""

  rc=$?
  return $rc
}

#
# Get ZK servers using ZK FLW conf command.
#
function zkFlwConfigGet() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local zkConnectString="$2"
  local _zkConnectStringOk=""
  local rc=$RC_FAILURE

  if [ ! -f "$FILE_ZK_CONFIG_SH" ]
  then
    echo2 "[$func] File <$FILE_ZK_CONFIG_SH> does not exist; returning <$rc>."
    return $rc
  fi

  source $FILE_ZK_CONFIG_SH

  if [ -z "$zkConnectString" ]
  then
    local funcToCall="zkCliConnectStringGet _zkConnectStringOk"
    $funcToCall

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
      return $rc
    fi
  else
    local funcToCall="whoIsOk _zkConnectStringOk $zkConnectString"
    $funcToCall

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
      return $rc
    fi
  fi

  local server=$(echo "$_zkConnectStringOk" | cut -d':' -f1)
  local port=$(echo "$_zkConnectStringOk" | cut -d':' -f2)
  local cmd="$CMD_FLW $server $port $FLW_CMD_CONF"

  echo2 "[$func] Running command <$cmd>."
  local _zkConfig=$(eval "$cmd" 2>&1)

  rc=$?
  echo2 "[$func] Command rc <$rc>, output <$_zkConfig>."

  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$cmd> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  _zkConfig=$(echo "$_zkConfig" | grep "$ZKCLI_SERVER_SEARCH_STRING")
  echo2 "[$func] According to ZK, current server configuration <\n$_zkConfig>."

  eval "$_result=\"$_zkConfig\""

  rc=$?
  return $rc
}

# ======================================================================
# zoo.cfg* functions.
# ======================================================================

#
# Create zoo.cfg.  Assumes zkConfigDatRead was called successfully.
#
function zooCfgCreate() {
  local func="${FUNCNAME[0]}"
  local stringReplace=""
  local rc=""

  echo2 "[$func]"

  cp -pfv $FILE_ZOO_CFG_TEMPLATE $FILE_ZOO_CFG
  chown $USER_ZOOKEEPER:$GROUP_ZOOKEEPER $FILE_ZOO_CFG
  chmod $PERMS_RW_R_R $FILE_ZOO_CFG

  stringReplace="$ZK_DATA_DIR"
  stringReplace="${stringReplace//\//\\/}"
  eval "sed -i 's/MFDD_dataDir/"$stringReplace"/g' $FILE_ZOO_CFG"

  stringReplace="$FILE_ZOO_CFG_DYNAMIC"
  stringReplace="${stringReplace//\//\\/}"
  eval "sed -i 's/MFDD_dynamicConfigFile/"$stringReplace"/g' $FILE_ZOO_CFG"

  eval "sed -i 's/MFDD_clientPort/"$ZK_PORT"/g' $FILE_ZOO_CFG"

  if [ ! -z "$ZK_PORT_SECURE" ]
  then
    cat >> $FILE_ZOO_CFG << EOF

# The port at which the clients will connect securely.
$ZOO_CFG_FIELD_SECURE_CLIENT_PORT=$ZK_PORT_SECURE
EOF
  fi

  rc=$?
  return $rc
}

#
# Create zoo.cfg.dynamic.  Assumes zkConfigDatRead was called successfully.
#
function zooCfgDynamicCreate() {
  local func="${FUNCNAME[0]}"
  local zkConnectString="$1"
  local rc=""

  local address="$CLIENT_PORT_ADDRESS"
  local portPeer=$ZK_PORT_PEER
  local portLeader=$ZK_PORT_LEADER

  if [ "$ZK_USES_PORT_SECURE" = "$STR_TRUE" ]
  then
    if [ "$ZK_USES_SSH_TUNNELS" = "$STR_TRUE" ]
    then
      address="$CLIENT_PORT_ADDRESS_TUNNEL"
      portPeer=$ZK_PORT_PEER_TUNNEL
      portLeader=$ZK_PORT_LEADER_TUNNEL
    fi
  fi

  local server="server.$ZK_MYID=$address:$portPeer:$portLeader:$ZK_CFG_DYNAMIC_ROLE_PARTICIPANT;$CLIENT_PORT_ADDRESS:$ZK_PORT"

  local zkConfig=""
  local funcToCall="zkFlwConfigGet zkConfig $zkConnectString"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>, myid <$ZK_MYID>."
    echo2 "[$func] Continuing; assuming 1st ZK in ensemble is being installed."
  fi

  > $FILE_ZOO_CFG_DYNAMIC
  chown $USER_ZOOKEEPER:$GROUP_ZOOKEEPER $FILE_ZOO_CFG_DYNAMIC
  chmod $PERMS_RW_R_R $FILE_ZOO_CFG_DYNAMIC

  if [ ! -z "$zkConfig" ]
  then
    echo "$zkConfig" >> $FILE_ZOO_CFG_DYNAMIC
  fi

  echo "$zkConfig" | grep -q "$server"

  rc=$?
  if [ $rc -eq $RC_SUCCESS ]
  then
    echo2 "[$func] Current server configuration <\n$zkConfig> already contains server entry <$server>; not adding duplicate server entry."
  else
    echo2 "[$func] Adding server entry <$server> to file <$FILE_ZOO_CFG_DYNAMIC>."
    echo "$server" >> $FILE_ZOO_CFG_DYNAMIC
  fi

  echo2 "[$func] Contents of file <$FILE_ZOO_CFG_DYNAMIC>:"
  cat $FILE_ZOO_CFG_DYNAMIC

  rc=$?
  return $rc
}

#
# Remove zoo.cfg* files.
#
function zooCfgRemove() {
  local func="${FUNCNAME[0]}"
  local rc=""

  echo2 "[$func]"
  rm -fv $FILE_ZOO_CFG*

  rc=$?
  return $rc
}

# ======================================================================
# ZK reconfiguration functions.
# ======================================================================

#
# Reconfigure ZK.
#
function zkReconfig() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local op="$2"
  local param="$3"
  local zkConnectString="$4"
  local _zkConnectStringOk=""
  local funcToCall=""
  local rc=$RC_FAILURE

  case "$op" in
    "$ZK_RECONFIG_OP_ADD" | "$ZK_RECONFIG_OP_REMOVE" | "$ZK_RECONFIG_OP_MEMBERS" | "$ZK_RECONFIG_OP_FILE")
      ;;

    *)
      echo2 "[$func] Unsupported operation <$op>; returning <$rc>."
      return $rc
  esac

  if [ -z "$zkConnectString" ]
  then
    funcToCall="zkCliConnectStringGet _zkConnectStringOk"
    $funcToCall

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
      return $rc
    fi
  else
    local funcToCall="whoIsOk _zkConnectStringOk $zkConnectString"
    $funcToCall

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
      return $rc
    fi
  fi

  local cmdZkCli="$CMD_ZKCLI -server $_zkConnectStringOk"
  local cmdReconfig="$CMD_ZKCLI_RECONFIG $op \"$param\""
  local cmd="$cmdZkCli; $cmdReconfig"

  local tryNum=1
  while [ $tryNum -le $ZKCLI_TRY_MAX ]
  do
    local jitter=$(($RANDOM % 30 + 1))
    sleep $jitter

    echo2 "[$func] Running command <$cmd>, try # <$tryNum>, jitter <$jitter>."
    local _zkConfig=$((eval "$cmdZkCli" << EOF
$cmdReconfig
$CMD_ZKCLI_QUIT
EOF
) 2>&1)

    rc=$?
    echo2 "[$func] Command rc <$rc>, output <$_zkConfig>."

    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$cmd> failed with return code <$rc>; retrying."

      tryNum=$(($tryNum + 1))
      continue
    fi

    echo "$_zkConfig" | grep -q "$ZKCLI_OUTPUT_BAD_ARGUMENTS"

    rc=$?
    if [ $rc -eq $RC_SUCCESS ]
    then
      rc=$RC_FAILURE
      echo2 "[$func] ZK CLI output contains <$ZKCLI_OUTPUT_BAD_ARGUMENTS>; returning <$rc>."

      return $rc
    fi

    _zkConfig=$(echo "$_zkConfig" | grep "$ZKCLI_SERVER_SEARCH_STRING")
    rc=$?

    echo2 "[$func] According to ZK, current server configuration <\n$_zkConfig>."

    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$cmd> returned no server configuration <$_zkConfig>; retrying."

      tryNum=$(($tryNum + 1))
      continue
    fi

    break
  done

  if [ $tryNum -gt $ZKCLI_TRY_MAX ]
  then
    echo2 "[$func] # tries <$tryNum> exceeded max tries <$ZKCLI_TRY_MAX>; returning <$rc>."
    return $rc
  fi

  echo2 "[$func] Command <$cmd> succeeded at try # <$tryNum>."

  eval "$_result=\"$_zkConfig\""

  rc=$?
  return $rc
}

#
# Add this server to ZK configuration.
#
function zkReconfigAdd() {
  local func="${FUNCNAME[0]}"
  local role="$1"
  local zkConnectString="$2"
  local funcToCall=""
  local rc=$RC_FAILURE

  if [ ! -f "$FILE_ZOO_CFG_DYNAMIC" ]
  then
    echo2 "[$func] File <$FILE_ZOO_CFG_DYNAMIC> does not exist; returning <$rc>."
    return $rc
  fi

  local count=$(grep -c "$CLIENT_PORT_ADDRESS" $FILE_ZOO_CFG_DYNAMIC)
  if [ $count -ne 1 ]
  then
    echo2 "[$func] File <$FILE_ZOO_CFG_DYNAMIC> unexpectedly contains <$count> entries with client port address <$CLIENT_PORT_ADDRESS>; returning <$rc>."
    return $rc
  fi

  local server=$(grep "$CLIENT_PORT_ADDRESS" $FILE_ZOO_CFG_DYNAMIC)
  local str1=$(echo "$server" | cut -d';' -f1 | cut -d':' -f-3)
  local str2=$(echo "$server" | cut -d';' -f2)

  if [ -z "$role" ]
  then
    role="$ZK_CFG_DYNAMIC_ROLE_PARTICIPANT"
  fi

  server="$str1:$role;$str2"

  local zkConfig=""
  local funcToCall="zkFlwConfigGet zkConfig $zkConnectString"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  if [ -z "$zkConfig" ]
  then
    rc=$RC_FAILURE
    echo2 "[$func] Call <$funcToCall> returned no server configuration <$zkConfig>; returning <$rc>."

    return $rc
  fi

  echo "$zkConfig" | grep -q "$server"

  rc=$?
  if [ $rc -eq $RC_SUCCESS ]
  then
    echo2 "[$func] Current server configuration <\n$zkConfig> already contains server entry <$server>; returning <$rc>."
    return $rc
  fi

  zkConfig=""
  funcToCall="zkReconfig zkConfig $ZK_RECONFIG_OP_ADD $server $zkConnectString"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
  fi

  return $rc
}

#
# Remove this server from ZK configuration.
#
function zkReconfigRemove() {
  local func="${FUNCNAME[0]}"
  local zkConnectString="$1"
  local funcToCall=""
  local rc=$RC_FAILURE

  if [ ! -f "$FILE_ZK_CONFIG_SH" ]
  then
    echo2 "[$func] File <$FILE_ZK_CONFIG_SH> does not exist; returning <$rc>."
    return $rc
  fi

  source $FILE_ZK_CONFIG_SH

  if [ ! -f "$ZK_FILE_MYID" ]
  then
    echo2 "[$func] File <$ZK_FILE_MYID> does not exist; returning <$rc>."
    return $rc
  fi

  local myid=$(cat $ZK_FILE_MYID)
  local zkConfig=""
  funcToCall="zkReconfig zkConfig $ZK_RECONFIG_OP_REMOVE $myid $zkConnectString"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
  fi

  return $rc
}

#
# Specify members of ZK configuration.
#
function zkReconfigMembers() {
  local func="${FUNCNAME[0]}"
  local servers="$1"
  local zkConnectString="$2"
  local funcToCall=""
  local rc=""

  local zkConfig=""
  funcToCall="zkReconfig zkConfig $ZK_RECONFIG_OP_MEMBERS $servers $zkConnectString"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
  fi

  return $rc
}

#
# Specify dynamic config file containing members of ZK configuration.
#
function zkReconfigFile() {
  local func="${FUNCNAME[0]}"
  local file="$1"
  local zkConnectString="$2"
  local funcToCall=""
  local rc=""

  zkConfig=""
  funcToCall="zkReconfig zkConfig $ZK_RECONFIG_OP_FILE $file $zkConnectString"
  $funcToCall

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Call <$funcToCall> failed with return code <$rc>; returning <$rc>."
  fi

  return $rc
}

# ======================================================================
# Crontab functions.
# ======================================================================

#
# Add crontab entry to periodically check zoo.cfg.dynamic for up-to-date ZK
# server configuration.
#
function zkCrontabAdd() {
  local func="${FUNCNAME[0]}"
  local fileTemp=$(mktemp)
  local rc=""

  crontab -l > $fileTemp

  echo2 "[$func] Current crontab (before change):"
  cat $fileTemp

  #
  # Remove existing crontab entry if found.
  #
  sed -i "/$FILENAME_ZOO_CFG_DYNAMIC_PARSE/d" $fileTemp

  cat >> $fileTemp << EOF
*/$CMD_ZOO_CFG_DYNAMIC_PARSE_CRONTAB_FREQ * * * * timeout $CMD_ZOO_CFG_DYNAMIC_PARSE_TIMEOUT $CMD_ZOO_CFG_DYNAMIC_PARSE > /dev/null 2>&1
EOF

  crontab $fileTemp

  echo2 "[$func] Current crontab (after change):"
  crontab -l

  echo2 "[$func]"
  rm -fv $fileTemp

  rc=$?
  return $rc
}

#
# Remove crontab entry to periodically check zoo.cfg.dynamic for up-to-date ZK
# server configuration.
#
function zkCrontabRemove() {
  local func="${FUNCNAME[0]}"
  local fileTemp=$(mktemp)
  local rc=""

  crontab -l > $fileTemp

  echo2 "[$func] Current crontab (before change):"
  cat $fileTemp

  sed -i "/$FILENAME_ZOO_CFG_DYNAMIC_PARSE/d" $fileTemp
  crontab $fileTemp

  echo2 "[$func] Current crontab (after change):"
  crontab -l

  echo2 "[$func]"
  rm -fv $fileTemp

  rc=$?
  return $rc
}

# ======================================================================
# Functions.
# ======================================================================

#
# Add link to ZK root directory for MFDD use.
#
function mfdd3ppLinkAdd() {
  local func="${FUNCNAME[0]}"
  local rc=""

  mkdir -pv $DIR_MFDD_3PP

  #
  # Ensure parent directories are world readable, executable
  #
  local path="$DIR_MFDD_3PP"
  while [ "$path" != "/" ]
  do
    chmod a+rx $path
    path=$(dirname $path)
  done

  echo2 "[$func]"
  ln -sfv $DIR_ZK_ROOT $LINK_ZOOKEEPER

  rc=$?
  return $rc
}

#
# Initialize ZK's myid.  Assumes zkConfigDatRead was called successfully.
#
function zkMyidInit() {
  local func="${FUNCNAME[0]}"
  local rc=$RC_FAILURE

  if [ ! -f "$FILE_ZK_CONFIG_SH" ]
  then
    echo2 "[$func] File <$FILE_ZK_CONFIG_SH> does not exist; returning <$rc>."
    return $rc
  fi

  source $FILE_ZK_CONFIG_SH

  #
  # Create ZK data directory.
  #
  echo2 "[$func] Creating ZK data directory <$ZK_DATA_DIR>."
  mkdir -pv $ZK_DATA_DIR
  chown -R $USER_ZOOKEEPER:$GROUP_ZOOKEEPER $ZK_DATA_DIR
  chmod $PERMS_RWX_RX_RX $ZK_DATA_DIR

  #
  # Ensure parent directories are world readable, executable
  #
  local path="$ZK_DATA_DIR"
  while [ "$path" != "/" ]
  do
    chmod a+rx $path
    path=$(dirname $path)
  done

  if [ -f "$ZK_FILE_MYID" ]
  then
    rc=$RC_SUCCESS
    echo2 "[$func] File <$ZK_FILE_MYID> exists; assuming ZK data exists so do not reinitialize; returning <$rc>."

    return $rc
  fi

  local cmd="sudo -u $USER_ZOOKEEPER bash -c \"$CMD_ZKSERVER_INITIALIZE --force --myid=$ZK_MYID\""
  echo2 "[$func] Running command <$cmd>."
  eval "$cmd"

  rc=$?
  return $rc
}

#
# Convert server configuration obtained via FLW conf, ZK CLI config or reconfig
# command into ZK connect string.
#
function zkConfigToZkConnectString() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local zkConfig="${@:2}"
  local rc=$RC_FAILURE

  zkConfig=$(echo "$zkConfig" | sed 's/^[ \t]*//;s/[ \t]*$//')
  if [ -z "$zkConfig" ]
  then
    echo2 "[$func] Empty server configuration <$zkConfig>; returning <$rc>."
    return $rc
  fi

  if [ ! -f "$FILE_ZK_CONFIG_SH" ]
  then
    echo2 "[$func] File <$FILE_ZK_CONFIG_SH> does not exist; returning <$rc>."
    return $rc
  fi

  source $FILE_ZK_CONFIG_SH

  local _zkConnectString=""
  for i in $zkConfig
  do
    local zkServer=$(echo "$i" | cut -d';' -f2 | cut -d':' -f1)
    zkServer="$zkServer:$ZK_CLI_PORT"

    if [ -z "$_zkConnectString" ]
    then
      _zkConnectString="$zkServer"
      continue
    fi

    _zkConnectString="$_zkConnectString,$zkServer"
  done

  echo2 "[$func] Server configuration <$zkConfig> converted into ZK connect string <$_zkConnectString>."

  eval "$_result=\"$_zkConnectString\""

  rc=$?
  return $rc
}

#
# Convert server configuration obtained via FLW conf, ZK CLI config or reconfig
# command into ZK SSH tunnel peers list, ultimately becoming value of
# ZK_SSH_TUNNEL_PEERS environment variable.
#
function zkConfigToZkSshTunnelPeers() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local zkConfig="${@:2}"
  local rc=$RC_FAILURE

  zkConfig=$(echo "$zkConfig" | sed 's/^[ \t]*//;s/[ \t]*$//')
  if [ -z "$zkConfig" ]
  then
    echo2 "[$func] Empty server configuration <$zkConfig>; returning <$rc>."
    return $rc
  fi

  if [ ! -f "$FILE_ZK_CONFIG_SH" ]
  then
    echo2 "[$func] File <$FILE_ZK_CONFIG_SH> does not exist; returning <$rc>."
    return $rc
  fi

  source $FILE_ZK_CONFIG_SH

  local _zkSshTunnelPeers=""
  for i in $zkConfig
  do
    local zkServer=$(echo "$i" | cut -d';' -f2 | cut -d':' -f1)
    local portTunnel=$(echo "$i" | cut -d':' -f2)
    zkServer="$zkServer:$portTunnel:$ZK_PORT_PEER"

    if [ -z "$_zkSshTunnelPeers" ]
    then
      _zkSshTunnelPeers="$zkServer"
      continue
    fi

    _zkSshTunnelPeers="$_zkSshTunnelPeers,$zkServer"
  done

  echo2 "[$func] Server configuration <$zkConfig> converted into ZK SSH tunnel peers list <$_zkSshTunnelPeers>."

  eval "$_result=\"$_zkSshTunnelPeers\""

  rc=$?
  return $rc
}

#
# Convert server configuration obtained via FLW conf, ZK CLI config or reconfig
# command into ZK SSH tunnel leaders list, ultimately becoming value of
# ZK_SSH_TUNNEL_LEADERS environment variable.
#
function zkConfigToZkSshTunnelLeaders() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local zkConfig="${@:2}"
  local rc=$RC_FAILURE

  zkConfig=$(echo "$zkConfig" | sed 's/^[ \t]*//;s/[ \t]*$//')
  if [ -z "$zkConfig" ]
  then
    echo2 "[$func] Empty server configuration <$zkConfig>; returning <$rc>."
    return $rc
  fi

  if [ ! -f "$FILE_ZK_CONFIG_SH" ]
  then
    echo2 "[$func] File <$FILE_ZK_CONFIG_SH> does not exist; returning <$rc>."
    return $rc
  fi

  source $FILE_ZK_CONFIG_SH

  local _zkSshTunnelLeaders=""
  for i in $zkConfig
  do
    local zkServer=$(echo "$i" | cut -d';' -f2 | cut -d':' -f1)
    local portTunnel=$(echo "$i" | cut -d':' -f3)
    zkServer="$zkServer:$portTunnel:$ZK_PORT_LEADER"

    if [ -z "$_zkSshTunnelLeaders" ]
    then
      _zkSshTunnelLeaders="$zkServer"
      continue
    fi

    _zkSshTunnelLeaders="$_zkSshTunnelLeaders,$zkServer"
  done

  echo2 "[$func] Server configuration <$zkConfig> converted into ZK SSH tunnel leaders list <$_zkSshTunnelLeaders>."

  eval "$_result=\"$_zkSshTunnelLeaders\""

  rc=$?
  return $rc
}

#
# Given ZK connect string, find 1st accessible ZK server, satisfying following
# conditions.
#   - Responds to "ruok" four-letter word with "imok".
#   - Does not respond to "stat" four-letter word with "This ZooKeeper instance
#     is not currently serving requests".
#
function whoIsOk() {
  local func="${FUNCNAME[0]}"
  local _result="$1"
  local zkConnectString="$2"
  local rc=$RC_FAILURE

  if [ ! -f "$FILE_ZK_CONFIG_SH" ]
  then
    echo2 "[$func] File <$FILE_ZK_CONFIG_SH> does not exist; returning <$rc>."
    return $rc
  fi

  source $FILE_ZK_CONFIG_SH

  if [ -z "$zkConnectString" ]
  then
    echo2 "[$func] Empty ZK connect string <$zkConnectString>; returning <$rc>."
    return $rc
  fi

  local zkConnectString2="${zkConnectString//,/$' '}"
  for i in $zkConnectString2
  do
    local server=$(echo "$i" | cut -d':' -f1)
    local port=$(echo "$i" | cut -d':' -f2)
    local cmd=""
    local output=""

    cmd="$CMD_FLW $server $port $FLW_CMD_RUOK"

    echo2 "[$func] Running command <$cmd>."
    output=$(eval "$cmd" 2>&1)

    rc=$?
    echo2 "[$func] Command rc <$rc>, output <$output>."

    if [ $rc -ne $RC_SUCCESS ]
    then
      continue
    fi

    echo "$output" | grep -q "$FLW_RESP_IMOK"

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      continue
    fi

    cmd="$CMD_FLW $server $port $FLW_CMD_STAT"

    echo2 "[$func] Running command <$cmd>."
    output=$(eval "$cmd" 2>&1)

    rc=$?
    echo2 "[$func] Command rc <$rc>, output <$output>."

    if [ $rc -ne $RC_SUCCESS ]
    then
      continue
    fi

    echo "$output" | grep -q "$FLW_RESP_NOT_SERVING_REQUESTS"

    rc=$?
    if [ $rc -eq $RC_SUCCESS ]
    then
      continue
    fi

    rc=$RC_SUCCESS
    echo2 "[$func] Found accessible ZK server <$i> in ZK connect string <$zkConnectString>; returning <$rc>."

    eval "$_result=\"$i\""
    return $rc
  done

  rc=$RC_FAILURE
  echo2 "[$func] No accessible ZK server found in ZK connect string <$zkConnectString>; returning <$rc>."

  return $rc
}

#
# Wait for ZK pending syncs to complete.
#
function zkPendingSyncsWait() {
  local func="${FUNCNAME[0]}"
  local cmd="$CMD_ZOOKEEPER zk_pending_syncs"
  local rc=$RC_FAILURE

  local tryNum=1
  while [ $tryNum -le $ZK_PENDING_SYNCS_TRY_MAX ]
  do
    sleep 3

    echo2 "[$func] Running command <$cmd>, try # <$tryNum>."
    output=$(eval "$cmd" 2>&1)

    rc=$?
    echo2 "[$func] Command rc <$rc>, output <$output>."

    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$func] Call <$cmd> failed with return code <$rc>; retrying."

      tryNum=$(($tryNum + 1))
      continue
    fi

    local pendingSyncs=$(echo "$output" | cut -d' ' -f3)

    if [ $pendingSyncs -ne 0 ]
    then
      echo2 "[$func] Pending syncs <$pendingSyncs>; syncing in progress; retrying."

      tryNum=$(($tryNum + 1))
      continue
    fi

    echo2 "[$func] Pending syncs <$pendingSyncs>; syncing completed; breaking."
    break
  done

  if [ $tryNum -gt $ZK_PENDING_SYNCS_TRY_MAX ]
  then
    echo2 "[$func] # tries <$tryNum> exceeded max tries <$ZK_PENDING_SYNCS_TRY_MAX>; returning <$rc>."
    return $rc
  fi

  echo2 "[$func] According to command <$cmd>, syncing completed at try # <$tryNum>."

  rc=$?
  return $rc
}

# In memory of BDS.
