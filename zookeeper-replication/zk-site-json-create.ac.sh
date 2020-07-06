#!/bin/bash

source /opt/mfdd/latest/base/bin/zk-conf-create-lib.sh

SERVICE_NAME_SUFFIX="_ZookeeperAccess"

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
  # Get application group database servers.
  #
  appGroupDbServers=$(mscli -c 'list-dn' | grep -E "^system=.+,partitionType=site,partition=$zk_siteId,PartitionDataType=LRU,AppGroupDbServer=.+$")

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Unable to get application group database servers from application configuration; exiting <$rc>."
    exit $rc
  fi

  echo2 "[$cmd] Application group database servers:"
  echo "$appGroupDbServers"

  #
  # Find application group database servers where application group's current
  # state is active.
  #
  for i in $appGroupDbServers
  do
    listEntry=$(mscli -c "list-entry --dn=$i")

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$cmd] Unable to perform list-entry for DN <$i>, continuing."
      continue
    fi

    echo2 "[$cmd] List entry for DN <$i>:"
    echo "$listEntry"

    attr="currentState"
    val=$(echo "$listEntry" | grep "$attr" | cut -d'"' -f2)

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$cmd] Unable to get attribute <$attr> from DN <$i>, continuing."
      continue
    fi

    if [ "$val" != "$STATE_ACTIVE" ]
    then
      continue
    fi

    attr="serverAG"
    val=$(echo "$listEntry" | grep "$attr" | cut -d'"' -f2 | cut -d'=' -f4)

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$cmd] Unable to get attribute <$attr> from DN <$i>, continuing."
      continue
    fi

    echo2 "[$cmd] Found DN <$i> with application group <$val>, current state <$STATE_ACTIVE>."

    #
    # Get VM instances (i.e., ZK servers) associated with above found active
    # application group.
    #
    vmInstances=$(mscli -c 'list-dn' | grep -E "^system=.+,app=LRU,appGroup=$val,appInstance=.+,VmInstance=.+$")

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$cmd] Unable to get VM instances associated with application group <$val>; continuing."
      continue
    fi

    echo2 "[$cmd] VM instances associated with application group <$val>:"
    echo "$vmInstances"

    #
    # Get ZK hosts using VM instances.
    #
    zk_hosts=()
    for j in $vmInstances
    do
      zk_server=$(echo "$j" | cut -d'=' -f$FIELD_ZK_SERVER)
      zk_hosts+=($zk_server)

      #
      # Set service name to LRU local configuration database.
      #
      serviceName="$SERVICE_NAME_CONFIG_DB"
      echo2 "[$cmd] Service name <$serviceName>."

      #
      # Get service ID using service name.
      #
      serviceId=$(mscli -c 'get-all-services' | grep "$serviceName")

      rc=$?
      if [ $rc -ne $RC_SUCCESS ]
      then
        echo2 "[$cmd] Unable to get service ID from service registry, service name <$serviceName>, ZK server <$zk_server>; continuing."
        continue
      fi

      echo2 "[$cmd] Service ID <$serviceId>."

      count=$(echo "$serviceId" | wc -l)
      if [ $count -ne $NUM_SERVICE_ID_EXPECTED ]
      then
        echo2 "[$cmd] Number of service IDs <$count> != <$NUM_SERVICE_ID_EXPECTED>, service name <$serviceName>, ZK server <$zk_server>; continuing."
        continue
      fi

      #
      # Get service location of ZK server using service ID.
      #
      serviceLocation=$(mscli -c 'get-service-locations --service='"$serviceId" | grep '{"address":"'"$zk_server\",")

      rc=$?
      if [ $rc -ne $RC_SUCCESS ]
      then
        echo2 "[$cmd] Unable to get service location from service registry, service ID <$serviceId>, ZK server <$zk_server>; continuing."
        continue
      fi

      echo2 "[$cmd] ZK server <$zk_server>, service location <$serviceLocation>."

      count=$(echo "$serviceLocation" | wc -l)
      if [ $count -ne $NUM_SERVICE_LOCATION_EXPECTED ]
      then
        echo2 "[$cmd] Number of service locations <$count> != <$NUM_SERVICE_LOCATION_EXPECTED>, service ID <$serviceId>, ZK server <$zk_server>; continuing."
        continue
      fi

      optionalData=$(echo "$serviceLocation" | cut -d'{' -f3 | cut -d'}' -f1)
      optionalData="${optionalData//,/$' '}"
      optionalData="${optionalData//\"}"
      echo2 "[$cmd] ZK server <$zk_server>, optional data <$optionalData>."

      for k in $optionalData
      do
        key=$(echo "$k" | cut -d':' -f1)
        val=$(echo "$k" | cut -d':' -f2)

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
  done
} 2>&1 | tee $log

#
# Exit return code is return code from 1st command in pipe, i.e., stdout &
# stderr redirection.  More info at
# https://unix.stackexchange.com/questions/186826/parent-script-continues-when-child-exits-with-non-zero-exit-code.
#
rc=${PIPESTATUS[0]}
exit $rc
