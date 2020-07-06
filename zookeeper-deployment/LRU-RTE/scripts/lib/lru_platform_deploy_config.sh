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

function error_exit
{
    rm -f /tmp/${shost}.$$.ini ${shost}
    print_msg "See log file ${MS_LOG_FILE} for details."
    exit 1
}

function run_rteconfig_02
{
    typeset routine="${PGM_NAME}.run_rteconfig_02"
    typeset status

    status=$(chkpoint_status ${routine} $MS_STATUS_FILE)
    [ "$status" = "$OK" ] && return $SUCCESS_RC

    print_msg "Run ${LRU_SCRIPT_PATH}/postdeploy/015_RunRteConfig_02.py"
    run_cmd "${LRU_SCRIPT_PATH}/postdeploy/015_RunRteConfig_02.py /tmp/${shost}.$$.ini ${shost}" || {
       print_msg "ERROR: Failure from ${LRU_SCRIPT_PATH}/postdeploy/015_RunRteConfig_02.py."
       return 1
    }
    register_chkpoint ${routine} $MS_STATUS_FILE $OK
    return 0
}

#
# Main
#
PGM_NAME=$(basename $0)
export MS_LOG_FILE="/var/log/$(echo $PGM_NAME | cut -f1 -d '.').log"
export MS_STATUS_FILE="/var/log/$(echo $PGM_NAME | cut -f1 -d '.').status"
touch $MS_LOG_FILE
touch $MS_STATUS_FILE
LRU_LIB_SCRIPT=$(readlink -f "$0")
LRU_LIB_PATH=$(dirname $LRU_LIB_SCRIPT)
LRU_SCRIPT_PATH=$(dirname $LRU_LIB_PATH)
LRU_VERSION_PATH=$(dirname $LRU_SCRIPT_PATH)
DVTYPE=$(echo $LRU_VERSION_PATH | awk -F"/" '{print $NF}' | cut -f1 -d'_')
shost=$(hostname -s)
CLUSTER=$1

# Run ${LRU_SCRIPT_PATH}/postdeploy/015_RunRteConfig_02.py
run_rteconfig_02 || error_exit

rm -f /tmp/${shost}.$$.ini

print_msg "Successfully completed configuration."
exit 0
