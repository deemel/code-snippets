#!/bin/bash

#set -x

RC_SUCCESS=0
RC_FAILURE=1
MIN_ARGS=0
MAX_ARGS=100
TIMESTAMP_FORMAT="%Y/%m/%d %H:%M:%S"
DIR_UM_OPENAM_AC_API="/opt/mfdd/um/um-access-control-api-openam"
DIR_UM_OPENAM_AC_API_LIB="$DIR_UM_OPENAM_AC_API/lib"
DIR_UM_OPENAM_AC_API_LIB_3PP="$DIR_UM_OPENAM_AC_API_LIB/3pp"
DIR_MFDD_UM_OPENAM_AC_API_LIB="/opt/mfdd/latest/um/lib"
command=$(basename $0)

function displayUsage() {
  echo
  echo "USAGE: $command [options...]"
  echo

  exit $RC_FAILURE
}

function initialize() {
  local timestamp=$(date +"$TIMESTAMP_FORMAT")

  echo
  echo "----------------------------------------------------------------------"
  echo "\"$command\" invoked on $timestamp."
  echo "----------------------------------------------------------------------"
  echo

  return $?
}

function finalize() {
  local timestamp=$(date +"$TIMESTAMP_FORMAT")

  echo
  echo "\"$command\" completed with status <$1> on $timestamp."
  echo

  exit $1
}

######################################################################
# main
######################################################################

if [ $# -lt $MIN_ARGS ]
then
  displayUsage
fi

if [ $# -gt $MAX_ARGS ]
then
  displayUsage
fi

initialize

if [ -d $DIR_UM_OPENAM_AC_API_LIB ]
then
  for i in $DIR_UM_OPENAM_AC_API_LIB/*.jar $DIR_UM_OPENAM_AC_API_LIB_3PP/*.jar
  do
    cp="$cp:$i"
  done
else
  if [ -d $DIR_MFDD_UM_OPENAM_AC_API_LIB ]
  then
    for i in $DIR_MFDD_UM_OPENAM_AC_API_LIB/*.jar
    do
      cp="$cp:$i"
    done
  else
    echo "Unable to construct classpath from <$DIR_UM_OPENAM_AC_API_LIB> or <$DIR_MFDD_UM_OPENAM_AC_API_LIB>; exiting."
    finalize $RC_FAILURE
  fi
fi

java -cp $cp com.jupiter.mfdd.um.accesscontrol.testtool.Main "$@"

finalize $?
