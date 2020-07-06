#!/bin/bash

#set -x

source /opt/adp/bin/base-common.sh

OP_HELP="--help"

TIMESTAMP_FORMAT_SUFFIX="%Y%m%d%H%M%S"

command="$(basename $0)"

function displayUsage() {
  local rc=$1

  echo
  echo "USAGE: $command"
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

  *)
    if [ ! -z $op ]
    then
      displayUsage $RC_FAILURE
    fi
    ;;
esac

initialize "$command"

cp=""
for i in $ADP_LIB/*.jar
do
  cp="$cp:$i"
done

java $JAVA_OPTS_GS_LOG_REST_SERVER -cp $cp -Dlogback.configurationFile=$ADP_CONF/gs-log-rest-server.logback.xml com.jupiter.adp.log.rest.Server
rc=$?

finalize "$command" $rc
