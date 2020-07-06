#!/bin/bash

DML=0

source /opt/mfdd/latest/base/bin/zk-conf-create-lib.sh

FILENAME_ZK_REPL_AGENT_CONF="zkReplicationAgent.conf"
DIR_ZK_REPL_AGENT_CONF="$DIR_ETC_OPT_MFDD/conf/zk"
FILE_ZK_REPL_AGENT_CONF_MO="$EECONF/zk/$FILENAME_ZK_REPL_AGENT_CONF.mo"
FILE_ZK_REPL_AGENT_CONF_SITE_MO="$EECONF/zk/$FILENAME_ZK_REPL_AGENT_CONF.site.mo"
FILE_ZK_REPL_AGENT_CONF="$DIR_ZK_REPL_AGENT_CONF/$FILENAME_ZK_REPL_AGENT_CONF"
LOG_ZK_REPL_AGENT_CONF_CREATE="$DIR_ZK_LOG/$(basename $0 | cut -d'.' -f1).log"

cmd="$(basename $0)"
timestamp="$(date +"%Y%m%d%H%M%S")"
log="$LOG_ZK_REPL_AGENT_CONF_CREATE.$timestamp"

{
  #
  # Ensure necessary directories exist.
  #
  dirCreate "$DIR_ZK_LOG"
  dirCreate "$DIR_ZK_REPL_AGENT_CONF"

  #
  # Create log with world read permissions.
  #
  touch $log
  chmod $PERMS_RW_R_R $log

  #
  # Remove older logs so disk won't fill up.
  #
  logsPrune $LOG_ZK_REPL_AGENT_CONF_CREATE $NUM_LOGS_MIN $NUM_LOGS_MAX

  rc=$RC_FAILURE
  for i in "$FILE_MO" "$FILE_LDAPACCESS" "$FILE_ZK_REPL_AGENT_CONF_MO" "$FILE_ZK_REPL_AGENT_CONF_SITE_MO"
  do
    if [ ! -f "$i" ]
    then
      echo2 "[$cmd] File <$i> does not exist; exiting <$rc>."
      exit $rc
    fi
  done

  source $FILE_MO

  #
  # Re-use period, step values if available from existing configuration file.
  #
  if [ -f "$FILE_ZK_REPL_AGENT_CONF" ]
  then
    zk_period=$(grep '"period":' $FILE_ZK_REPL_AGENT_CONF | cut -d':' -f2 | cut -d',' -f1)
    zk_step=$(grep '"step":' $FILE_ZK_REPL_AGENT_CONF | cut -d':' -f2 | cut -d',' -f1)
  fi

  if [ ! -z "$zk_period" ]
  then
    echo2 "[$cmd] ZK replication agent period <$zk_period>."
  fi

  if [ ! -z "$zk_step" ]
  then
    echo2 "[$cmd] ZK replication agent step <$zk_step>."
  fi

  #
  # Get ZK server.
  #
  zk_server=$(hostname -s)
  echo2 "[$cmd] ZK server <$zk_server>."

  #
  # Get VM instance using ZK server.
  #
  vmInstance=$(mscli -c 'list-dn' | grep -E "^system=.+,app=LRU,appGroup=.+,VmInstance=$zk_server$")

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    echo2 "[$cmd] Unable to get VM instance from application configuration, ZK server <$zk_server>; exiting <$rc>."
    exit $rc
  fi

  echo2 "[$cmd] VM instance <$vmInstance>."

  count=$(echo "$vmInstance" | wc -l)
  if [ $count -ne $NUM_VM_INSTANCE_EXPECTED ]
  then
    rc=$RC_FAILURE

    echo2 "[$cmd] Number of VM instances <$count> != <$NUM_VM_INSTANCE_EXPECTED>, ZK server <$zk_server>; exiting <$rc>."
    exit $rc
  fi

  #
  # Get application group using VM instance.
  #
  appGroup=$(echo "$vmInstance" | cut -d',' -f$FIELD_APP_GROUP | cut -d'=' -f2)
  echo2 "[$cmd] Application group <$appGroup>."

if [ $DML -eq 1 ]
then
  appGroup="LRU-BEF1"
  echo2 "[$cmd] Application group <$appGroup> (DML hack)."
fi

  #
  # Get application group database servers.
  #
  appGroupDbServers=$(mscli -c 'list-dn' | grep -E "^system=.+,partitionType=site,partition=.+,PartitionDataType=LRU,AppGroupDbServer=.+$")

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
  # state is passive.
  #
  zkSiteFragmentFiles=()
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

    attr="serverAG"
    val=$(echo "$listEntry" | grep "$attr" | cut -d'"' -f2 | cut -d'=' -f4)

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$cmd] Unable to get attribute <$attr> from DN <$i>, continuing."
      continue
    fi

    if [ "$val" != "$appGroup" ]
    then
      continue
    fi

    attr="currentState"
    val=$(echo "$listEntry" | grep "$attr" | cut -d'"' -f2)

    rc=$?
    if [ $rc -ne $RC_SUCCESS ]
    then
      echo2 "[$cmd] Unable to get attribute <$attr> from DN <$i>, continuing."
      continue
    fi

    if [ "$val" != "$STATE_PASSIVE" ]
    then
      continue
    fi

    echo2 "[$cmd] Found DN <$i> with application group <$appGroup>, current state <$STATE_PASSIVE>."

    #
    # Get application group database server peers, i.e., peers of above found
    # DN.
    #
    appGroupDbServer="$(echo "$i" | rev | cut -d'=' -f2- | rev)="
    appGroupDbServerPeers=$(echo "$appGroupDbServers" | grep -v "$i" | grep "$appGroupDbServer")

    echo2 "[$cmd] Application group database server peers:"
    echo "$appGroupDbServerPeers"

    #
    # Find application group database servers where application group's current
    # state is active.
    #
    for j in $appGroupDbServerPeers
    do
      listEntry=$(mscli -c "list-entry --dn=$j")

      rc=$?
      if [ $rc -ne $RC_SUCCESS ]
      then
        echo2 "[$cmd] Unable to perform list-entry for DN <$j>, continuing."
        continue
      fi

      echo2 "[$cmd] List entry for DN <$j>:"
      echo "$listEntry"

      attr="currentState"
      val=$(echo "$listEntry" | grep "$attr" | cut -d'"' -f2)

      rc=$?
      if [ $rc -ne $RC_SUCCESS ]
      then
        echo2 "[$cmd] Unable to get attribute <$attr> from DN <$j>, continuing."
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
        echo2 "[$cmd] Unable to get attribute <$attr> from DN <$j>, continuing."
        continue
      fi

      echo2 "[$cmd] Found DN <$j> with application group <$val>, current state <$STATE_ACTIVE>."

      #
      # Get site ID associated with above found active application group.
      #
      siteList=$(mscli -c 'list-dn' | grep -E "^system=.+,app=LRU,appGroup=$val,SiteList=.+$")

      rc=$?
      if [ $rc -ne $RC_SUCCESS ]
      then
        echo2 "[$cmd] Unable to get site list associated with application group <$val>; continuing."
        continue
      fi

      echo2 "[$cmd] Site list DN <$siteList> associated with application group <$val>."

      listEntry=$(mscli -c "list-entry --dn=$siteList")

      echo2 "[$cmd] List entry for DN <$siteList>:"
      echo "$listEntry"

      zk_siteId=$(echo "$listEntry" | grep "siteDNs" | cut -d'"' -f2 | cut -d',' -f2 | cut -d'=' -f2)
      echo2 "[$cmd] Site ID <$zk_siteId> associated with application group <$val>."

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
      for i in $vmInstances
      do
        zk_server=$(echo "$i" | cut -d'=' -f$FIELD_ZK_SERVER)
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
      # Create ZK site fragment for zkReplicationAgent.conf.
      #
      fileTemp="$(mktemp)"

      cat $FILE_ZK_REPL_AGENT_CONF_SITE_MO | mo > $fileTemp

      rc=$?
      if [ $rc -ne $RC_SUCCESS ]
      then
        echo2 "[$cmd] Unable to create ZK site fragment file <$fileTemp>; continuing."
        continue
      fi

      sed -i 's/,]/]/g' $fileTemp

      echo2 "[$cmd] Contents of ZK site fragment file <$fileTemp>:"
      cat $fileTemp

      zkSiteFragmentFiles+=($fileTemp)
    done
  done

  #
  # Insert ZK site fragments into zkReplicationAgent.conf mustache template.
  #
  fileTemp="$(mktemp)"

  echo
  cp -fv $FILE_ZK_REPL_AGENT_CONF_MO $fileTemp

  length=${#zkSiteFragmentFiles[@]}
  if [ $length -gt 0 ]
  then
    for i in ${zkSiteFragmentFiles[@]}
    do
      #
      # https://stackoverflow.com/questions/11243102/using-sed-to-insert-file-content
      #
      sed -i -e "/]/{
h
r $i
g
N}" $fileTemp

      echo
      rm -fv $i
    done

    fileTemp2="$(mktemp)"
    tac $fileTemp | sed '0,/},/s//}/' | tac > $fileTemp2

    echo
    mv -fv $fileTemp2 $fileTemp
  fi

  echo2 "[$cmd] Contents of mustache template <$fileTemp>:"
  cat $fileTemp

  #
  # Create zkReplicationAgent.conf.
  #
  zkConfigCreate "$fileTemp" "$FILE_ZK_REPL_AGENT_CONF"

  echo
  rm -fv $fileTemp

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
