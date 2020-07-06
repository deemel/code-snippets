#!/bin/bash

source /opt/mfdd/latest/base/bin/zk-conf-create-lib.sh

SERVICE_NAME_SUFFIX="_ZookeeperAccess"
FIELD_ZK_SERVER=4

FILENAME_ZK_SITE_JSON="zk-site.json"
DIR_ZK_SITE_JSON="$DIR_ETC_OPT_MFDD/cfg"
FILE_ZK_SITE_JSON_MO="$EECONF/zk/$FILENAME_ZK_SITE_JSON.mo"
FILE_ZK_SITE_JSON="$DIR_ZK_SITE_JSON/$FILENAME_ZK_SITE_JSON"
LOG_ZK_SITE_JSON_CREATE="$DIR_ZK_LOG/$(basename $0 | cut -d'.' -f1).log"

cmd="$(basename $0)"
timestamp="$(date +"%Y%m%d%H%M%S")"
log="$LOG_ZK_SITE_JSON_CREATE.$timestamp"

{
  #
  # Ensure necessary directories exist.
  #
  dirCreate "$DIR_ZK_LOG"
  dirCreate "$DIR_ZK_SITE_JSON"

  #
  # Create log with world read permissions.
  #
  touch $log
  chmod $PERMS_RW_R_R $log

  #
  # Remove older logs so disk won't fill up.
  #
  logsPrune $LOG_ZK_SITE_JSON_CREATE $NUM_LOGS_MIN $NUM_LOGS_MAX

  rc=$RC_FAILURE
  for i in "$FILE_MO" "$FILE_LDAPACCESS" "$FILE_ZK_SITE_JSON_MO"
  do
    if [ ! -f "$i" ]
    then
      echo2 "[$cmd] File <$i> does not exist; exiting <$rc>."
      exit $rc
    fi
  done

  source $FILE_MO

  #
  # Get site ID.
  #
  zk_siteId=$(grep "^LOCAL_SITE=" $FILE_LDAPACCESS)

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Unable to find local site ID in file <$FILE_LDAPACCESS>; exiting <$rc>."
    exit $rc
  fi

  zk_siteId=$(echo "$zk_siteId" | cut -d'=' -f2)
  echo2 "[$cmd] Site ID <$zk_siteId>."

  #
  # If zk-site.json does not exist, create initial version.
  #
  if [ ! -f "$FILE_ZK_SITE_JSON" ]
  then
    if [ -f "$FILE_ZK_CONFIG_DAT" ]
    then
      echo2 "[$cmd] File <$FILE_ZK_SITE_JSON> does not exist; creating initial version from file <$FILE_ZK_CONFIG_DAT>."

      source $FILE_ZK_CONFIG_DAT

      zk_port=$ZK_PORT_SECURE
      zk_hosts=()
      zkServers="${ZK_SERVERS//,/$' '}"

      for i in $zkServers
      do
        zk_hosts+=($i)
      done

      IFS=$'\n'
      zk_hosts=($(sort <<< "${zk_hosts[*]}"))
      unset IFS

      echo2 "[$cmd] Site ID <$zk_siteId>."
      echo2 "[$cmd] ZK hosts <${zk_hosts[@]}>."
      echo2 "[$cmd] ZK client port number <$zk_port>."

      #
      # Create zk-site.json
      #
      zkConfigCreate "$FILE_ZK_SITE_JSON_MO" "$FILE_ZK_SITE_JSON"

      rc=$?
      exit $rc
    fi
  fi

  #
  # Get service name using site ID.
  #
  serviceName="$zk_siteId$SERVICE_NAME_SUFFIX"
  echo2 "[$cmd] Service name <$serviceName>."

  #
  # Get service ID using service name.
  #
  serviceId=$(mscli -c 'get-all-services' | grep "$serviceName")

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Unable to get service ID from service registry, service name <$serviceName>; exiting <$rc>."
    exit $rc
  fi

  echo2 "[$cmd] Service ID <$serviceId>."

  count=$(echo "$serviceId" | wc -l)
  if [ $count -ne $NUM_SERVICE_ID_EXPECTED ]
  then
    rc=$RC_FAILURE

    echo2 "[$cmd] Number of service IDs <$count> != <$NUM_SERVICE_ID_EXPECTED>, service name <$serviceName>; exiting <$rc>."
    exit $rc
  fi

  #
  # Get service locations using service ID.
  #
  serviceLocations=$(mscli -c 'get-service-locations --service='"$serviceId" | grep '{"address":"')

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Unable to get service locations from service registry, service ID <$serviceId>; exiting <$rc>."
    exit $rc
  fi

  echo2 "[$cmd] Service locations:"
  echo "$serviceLocations"

  #
  # Get ZK hosts using service locations. Sort ZK hosts so that downstream
  # processes don't get tricked into processing zk-site.json with same set of
  # ZK hosts as previous set but in different order.
  #
  zk_hosts=()
  for i in $serviceLocations
  do
    zk_server=$(echo "$i" | cut -d'"' -f$FIELD_ZK_SERVER)
    zk_hosts+=($zk_server)

    optionalData=$(echo "$i" | cut -d'{' -f3 | cut -d'}' -f1)
    optionalData="${optionalData//,/$' '}"
    optionalData="${optionalData//\"}"
    echo2 "[$cmd] ZK server <$zk_server>, optional data <$optionalData>."

    for j in $optionalData
    do
      key=$(echo "$j" | cut -d':' -f1)
      val=$(echo "$j" | cut -d':' -f2)

      case "$key" in
        "$FIELD_NAME_IS_UPGRADE")
          echo2 "[$cmd] ZK server <$zk_server>, is upgrade <$val>."

          if [ $val -eq $BOOL_TRUE ]
          then
            zk_siteId=""
            echo2 "[$cmd] ZK server <$zk_server>, site ID <$zk_siteId>."
          fi
          ;;

        "$FIELD_NAME_PORT_ZK")
          zk_port=$val
          echo2 "[$cmd] ZK server <$zk_server>, ZK client port number <$zk_port>."
          ;;

        *)
          continue
      esac
    done
  done

  IFS=$'\n'
  zk_hosts=($(sort <<< "${zk_hosts[*]}"))
  unset IFS

  echo2 "[$cmd] Site ID <$zk_siteId>."
  echo2 "[$cmd] ZK hosts <${zk_hosts[@]}>."
  echo2 "[$cmd] ZK client port number <$zk_port>."

  #
  # Create zk-site.json
  #
  zkConfigCreate "$FILE_ZK_SITE_JSON_MO" "$FILE_ZK_SITE_JSON"

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
