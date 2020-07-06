#!/bin/bash

function run_cmd
{
    typeset cmd="$1"
    typeset my_pid
    typeset my_status

    $cmd >> $MS_LOG_FILE 2>&1 &
    my_pid=$!
    # print dots to prevent ssh connection timeout.
    while true
    do
        ps -ef | grep " ${my_pid} " | grep -v grep > /dev/null
        if [ $? -eq 0 ]
        then
          dot="....."
          for i in {1..5}; do
              echo -ne "\rPlease wait${dot:0:$i}"
              sleep .9
          done
        else
            echo ""
           break
        fi
    done
    wait $my_pid
    my_status=$?
    [ $my_status -ne 0 ] && {
        return 1
    }
    return 0
}

PGM_NAME=$(basename $0)
MS_LOG_FILE="/var/log/$(echo $PGM_NAME | cut -f1 -d '.').log"
touch $MS_LOG_FILE
LRU_LIB_SCRIPT=$(readlink -f "$0")
LRU_LIB_PATH=$(dirname $LRU_LIB_SCRIPT)
LRU_SCRIPT_PATH=$(dirname $LRU_LIB_PATH)
LRU_VERSION_PATH=$(dirname $LRU_SCRIPT_PATH)
DVTYPE=$(echo $LRU_VERSION_PATH | awk -F"/" '{print $NF}' | cut -f1 -d'_')
shost=$(hostname -s)
CLUSTER=$1

echo "Run ${LRU_SCRIPT_PATH}/postdeploy/015_RunRteConfig_01.py" | tee -a $MS_LOG_FILE
run_cmd "${LRU_SCRIPT_PATH}/postdeploy/015_RunRteConfig_01.py /tmp/${shost}.$$.ini ${shost}" || {
   echo "ERROR: Failure from ${LRU_SCRIPT_PATH}/postdeploy/015_RunRteConfig_01.py."
   echo "See log file ${MS_LOG_FILE} for details." | tee -a $MS_LOG_FILE
   rm -f /tmp/${shost}.$$.ini
   exit 1
}

rm -f /tmp/${shost}.$$.ini
echo "Successfully completed configuration." | tee -a $MS_LOG_FILE
exit 0
