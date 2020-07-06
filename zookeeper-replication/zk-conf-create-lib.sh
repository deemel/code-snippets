#!/bin/bash

RC_SUCCESS=0
RC_FAILURE=1
BOOL_FALSE=0
BOOL_TRUE=1

LOG_DATE_FORMAT="%Y-%m-%d %H:%M:%S"
PERMS_RW_R_R=644
NUM_LOGS_MIN=10
NUM_LOGS_MAX=60
ZK_PORT_DEF=2281
ZK_PERIOD_DEF=600
ZK_STEP_DEF=300
NUM_VM_INSTANCE_EXPECTED=1
NUM_SERVICE_ID_EXPECTED=1
NUM_SERVICE_LOCATION_EXPECTED=1
STATE_ACTIVE="Active"
STATE_PASSIVE="Passive"
SERVICE_NAME_CONFIG_DB="LRU_LocalConfigDB"
FIELD_NAME_IS_UPGRADE="is_upgrade"
FIELD_NAME_PORT_ZK="port_zk"
FIELD_SITE_LIST=5
FIELD_APP_GROUP=3
FIELD_ZK_SERVER=6

DIR_ZK_LOG="/var/log/zookeeper"
DIR_ETC_OPT="/etc/opt"
DIR_ETC_OPT_MFDD="$DIR_ETC_OPT/mfdd"
FILE_MO="/usr/local/bin/mo"
FILE_MFDD_ENV="/opt/mfdd/latest/base/bin/mfddEnv"
FILE_ZK_CONFIG_DAT="/usr/etc/zookeeperConfiguration.dat"
FILE_LDAPACCESS="$DIR_ETC_OPT/lru/ldap/.ldapaccess"

source $FILE_MFDD_ENV

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
# Create directory with world read, execute permissions.
#
function dirCreate() {
  local func="${FUNCNAME[0]}"
  local path="$1"
  local rc=""

  mkdir -p $path

  while [ "$path" != "/" ]
  do
    chmod a+rx $path
    path=$(dirname $path)
  done

  rc=$?
  return $rc
}

#
# Prune logs.
#
function logsPrune() {
  local func="${FUNCNAME[0]}"
  local log="$1"
  local numLogsMin=$2
  local numLogsMax=$3
  local rc=""

  local logs=$(find $(dirname $log) -type f -name "$(basename $log).*" | sort)
  local count=$(echo "$logs" | wc -l)

  if [ $count -ge $numLogsMax ]
  then
    local numLogsRemove=$(($count - $numLogsMin))
    local logsRemove=$(echo $logs | cut -d' ' -f1-$numLogsRemove)

    echo2 "[$func] Removing <$numLogsRemove> oldest $log.* logs."

    local i=""
    for i in $logsRemove
    do
      rm -fv $i
    done
  fi

  rc=$?
  return $rc
}

#
# Create ZK configuration file.
#
function zkConfigCreate() {
  local func="${FUNCNAME[0]}"
  local fileZkConfigMo="$1"
  local fileZkConfig="$2"
  local fileTemp="$(mktemp)"
  local rc=""

  cat $fileZkConfigMo | mo > $fileTemp

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$func] Unable to create temporary ZK configuration file <$fileTemp>; exiting <$rc>."
    exit $rc
  fi

  sed -i 's/,]/]/g' $fileTemp

  rc=$RC_FAILURE
  if [ -f "$fileZkConfig" ]
  then
    echo2 "[$func] Differences between proposed & current versions of file <$fileZkConfig>:"

    diff $fileTemp $fileZkConfig
    rc=$?
  fi

  if [ $rc -eq $RC_SUCCESS ]
  then
    echo2 "[$func] No changes needed in file <$fileZkConfig>; continuing."

    echo
    rm -fv $fileTemp
  else
    echo
    mv -fv $fileTemp $fileZkConfig
    chmod $PERMS_RW_R_R $fileZkConfig
  fi

  echo
  ls -l $fileZkConfig

  echo2 "[$cmd] Contents of file <$fileZkConfig>:"
  cat $fileZkConfig

  rc=$?
  return $rc
}
