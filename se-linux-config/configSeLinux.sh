#!/bin/bash

source /opt/mfdd/latest/base/bin/mfddEnv
source /opt/mfdd/latest/base/rte/rteCommonFuncs.sh

RC_SUCCESS=0
RC_FAILURE=1
MIN_ARGS=0
MAX_ARGS=1

PARAM_HELP="-h"
PARAM_INSTALL="-install"
PARAM_UNINSTALL="-uninstall"

USER="root"
NAME_LOCALPOLICY="localpolicy"
DIR_LOCALPOLICY="$EECONF/selinux"
MAKEFILE_SELINUX="/usr/share/selinux/devel/Makefile"
FILE_LOCALPOLICY_TE="$DIR_LOCALPOLICY/$NAME_LOCALPOLICY.te"
FILE_LOCALPOLICY_PP="$DIR_LOCALPOLICY/$NAME_LOCALPOLICY.pp"
CMD_LOCALPOLICY_MAKE="make -f $MAKEFILE_SELINUX $(basename $FILE_LOCALPOLICY_PP)"
CMD_LOCALPOLICY_INSTALL="semodule -v -i $FILE_LOCALPOLICY_PP"
CMD_LOCALPOLICY_UNINSTALL="semodule -r $NAME_LOCALPOLICY"
TIMESTAMP_FORMAT="%Y/%m/%d %H:%M:%S"

command=$(basename $0)
pid=$$

function displayUsage() {
  echo
  echo "USAGE: $command {$PARAM_INSTALL | $PARAM_UNINSTALL}"
  echo

  exit $RC_FAILURE
}

function initialize() {
  local timestamp=$(date +"$TIMESTAMP_FORMAT")

  echo
  echo "----------------------------------------------------------------------"
  echo "$command $action invoked on $timestamp."
  echo "----------------------------------------------------------------------"
  echo

  return $?
}

function finalize() {
  local timestamp=$(date +"$TIMESTAMP_FORMAT")

  echo
  echo "$command $action completed with status <$1> on $timestamp."
  echo

  exit $1
}

function runCommand() {
  echo "Running command <$@>."
  $@
  
  if [ $? -ne $RC_SUCCESS ]
  then
    echo "Command <$@> failed; returning <$RC_FAILURE>."
    return $RC_FAILURE
  fi

  return $RC_SUCCESS
}

function install() {
  for i in $MAKEFILE_SELINUX $FILE_LOCALPOLICY_TE
  do
    if [ ! -f $i ]
    then
      echo "File <$i> does not exist; returning <$RTE_FAIL_MISSING_ENV>."
      return $RTE_FAIL_MISSING_ENV
    fi
  done

  runCommand "cd $DIR_LOCALPOLICY"
  if [ $? -ne $RC_SUCCESS ]
  then
    return $RC_FAILURE
  fi

  runCommand "$CMD_LOCALPOLICY_MAKE"
  if [ $? -ne $RC_SUCCESS ]
  then
    return $RC_FAILURE
  fi

  runCommand "$CMD_LOCALPOLICY_INSTALL"
  errCode=$?
  if [ $errCode -ne $RC_SUCCESS ]
  then
    echo "ERRROR: semodule install failed with code $errCode"
    #return $RC_FAILURE
  fi
  
  if [ $errCode -eq $RC_SUCCESS ]; then
    for i in $(ls -1)
    do
      if [ "$i" = "$(basename $FILE_LOCALPOLICY_TE)" ]
      then
        continue
      fi
      rm -rf $i
    done
  fi

  return $RC_SUCCESS
}

function uninstall() {
  runCommand "$CMD_LOCALPOLICY_UNINSTALL"
  if [ $? -ne $RC_SUCCESS ]
  then
    return $RC_FAILURE
  fi

  return $RC_SUCCESS
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

if [ $# -eq 1 ]
then
  if [ "$1" = "$PARAM_HELP" ]
  then
    displayUsage
  fi
fi

action="$1"

case "$action" in
  "$PARAM_INSTALL")
    ;;

  "$PARAM_UNINSTALL")
    ;;

  *)
    displayUsage
    ;;
esac

if [ "$(whoami)" != "$USER" ]
then
  echo "Must be user <$USER> to run this script; exiting."
  exit $RC_FAILURE
fi

initialize

case "$action" in
  "$PARAM_INSTALL")
    install
    ;;

  "$PARAM_UNINSTALL")
    uninstall
    ;;
esac

finalize $?
