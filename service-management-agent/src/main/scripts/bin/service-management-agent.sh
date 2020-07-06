#!/bin/bash

#set -x

source /opt/adp/bin/base-common.sh

OP_HELP="--help"
OP_REGISTER="--register"
OP_DEREGISTER="--deregister"
OP_DISCOVER="--discover"
OP_LOGSTASH_CONFIG="--logstash-config"

command="$(basename $0)"

function displayUsage() {
  local rc=$1

  echo
  echo "USAGE: $command $OP_REGISTER"
  echo "       $command $OP_DEREGISTER"
  echo "       $command $OP_DISCOVER {service name} {tag} [environment variable]"
  echo "       $command $OP_LOGSTASH_CONFIG {service name} {tag} {environment variable} {Logstash config template file} {Logstash config file} [discover interval]"
  echo

  exit $rc
}

######################################################################
# main
######################################################################

op="$1"
case "$op" in
  "$OP_HELP")
    displayUsage $RC_SUCCESS
    ;;

  "$OP_REGISTER")
    if [ $# -ne 1 ]
    then
      displayUsage $RC_FAILURE
    fi
    ;;

  "$OP_DEREGISTER")
    if [ $# -ne 1 ]
    then
      displayUsage $RC_FAILURE
    fi
    ;;

  "$OP_DISCOVER")
    if [[ $# -lt 3 || $# -gt 4 ]]
    then
      displayUsage $RC_FAILURE
    fi
    ;;

  "$OP_LOGSTASH_CONFIG")
    if [[ $# -lt 6 || $# -gt 7 ]]
    then
      displayUsage $RC_FAILURE
    fi
    ;;

  *)
    displayUsage $RC_FAILURE
    ;;
esac

command="$command $op"
initialize "$command"

cp=""
for i in $ADP_LIB/*.jar
do
  cp="$cp:$i"
done

java $JAVA_OPTS_SMA -cp $cp -Dlogback.configurationFile=$ADP_CONF/service-management-agent.logback.xml com.jupiter.adp.sd.Client $@
rc=$?

finalize "$command" $rc
