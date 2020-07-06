#!/bin/bash

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

RC_SUCCESS=${rc.success}
RC_FAILURE=${rc.failure}

USER_ROOT="${user.root}"

DIR_MFDD_BIN="${dir.mfdd.bin}"
DIR_TUNNELS="${dir.tunnels}"

# ======================================================================
# Constants.
# ======================================================================

MIN_ARGS=1
MAX_ARGS=1

PHASE_PRE="pre"
PHASE_POST="post"

CMD="$(basename $0)"
USER=$(whoami)
HOSTNAME=$(hostname -s)
DIR_OPT_TUNNELS="/opt/tunnels"
CMD_CLI="$DIR_MFDD_BIN/cli -c"
SERVICE_ZK_SSH_TUNNEL_PREFIX="zks-"
SERVICE_PROFILE_ZK_SSH_TUNNELS="ZKSSH-SERVER"
CLI_UPDATE_SERVICEPROFILE_OUTPUT_CANNOT_DEALLOCATE="Cannot de-allocate profile as one of the service in profile is currently running"

# ======================================================================

function displayUsage() {
  local func="${FUNCNAME[0]}"

  echo
  echo "This script is intended to be used when upgrading a non-dynamic ZK"
  echo "server to a dynamic ZK server (e.g., from ZK in MFDD 2.63 to ZK in MFDD"
  echo "3.11.0)."
  echo
  echo "The \"$PHASE_PRE\" option is intended to ensure SM-managed ZK SSH"
  echo "tunnel services continue to work during upgrade.  It should be called"
  echo "before upgrade."
  echo
  echo "The \"$PHASE_POST\" option is intended to remove SM-managed ZK SSH"
  echo "tunnel services.  It should be called after upgrade."
  echo
  echo "USAGE: $CMD {$PHASE_PRE|$PHASE_POST}"
  echo

  exit $RC_FAILURE
}

function phasePre() {
  local func="${FUNCNAME[0]}"
  local rc=$RC_FAILURE

  if [ -d "$DIR_OPT_TUNNELS" ]
  then
    echo
    echo "[$func] Directory <$DIR_OPT_TUNNELS> exists; returning <$rc>."
    return $rc
  fi

  ln -sfv $DIR_TUNNELS $DIR_OPT_TUNNELS

  rc=$?
  return $rc
}

function phasePost() {
  local func="${FUNCNAME[0]}"
  local rc=$RC_FAILURE
  local cmd=""
  local output=""

  #
  # Remove /opt/tunnels link BEFORE stopping & disabling ZK SSH tunnel services
  # via CLI because underlying tunnels need to remain running afterwards.
  #
  rm -fv $DIR_OPT_TUNNELS

  cmd="$CMD_CLI \"show-service\""
  output=$(eval "$cmd")

  rc=$?
  echo
  echo -e "[$func] Command <$cmd>, rc <$rc>, output <\n$output>."

  if [ $rc -ne $RC_SUCCESS ]
  then
    echo
    echo "[$func] Command <$cmd> failed with return code <$rc>; returning <$rc>."
    return $rc
  fi

  output=$(echo "$output" | grep "$SERVICE_ZK_SSH_TUNNEL_PREFIX" | sort)

  rc=$?
  echo
  echo -e "[$func] According to CLI, ZK SSH tunnel services <\n$output>."

  if [ $rc -ne $RC_SUCCESS ]
  then
    echo
    echo "[$func] No ZK SSH tunnel services found; returning <$rc>."
    return $rc
  fi

  if [ ! -z "$output" ]
  then
    for i in $output
    do
      echo
      $CMD_CLI "stop-service --node=$HOSTNAME --service=$i"

      echo
      $CMD_CLI "update-service --node=$HOSTNAME --service=$i --disable"
    done
  fi

  echo
  $CMD_CLI "update-serviceprofile --node=$HOSTNAME --serviceprofile=$SERVICE_PROFILE_ZK_SSH_TUNNELS --deallocate"

  echo
  $CMD_CLI "show-service-status --node=$HOSTNAME"

  echo
  $CMD_CLI "show-serviceprofile --node=$HOSTNAME"

  rc=$?
  return $rc
}

# ======================================================================
# Main.
# ======================================================================

if [ "$USER" != "$USER_ROOT" ]
then
  echo "[$CMD] User <$USER> is not <$USER_ROOT>; exiting <$RC_FAILURE>."
  exit $RC_FAILURE
fi

if [ $# -lt $MIN_ARGS ]
then
  displayUsage
fi

if [ $# -gt $MAX_ARGS ]
then
  displayUsage
fi

phase="$1"

case "$phase" in
  "$PHASE_PRE")
    phasePre
    rc=$?
    ;;

  "$PHASE_POST")
    phasePost
    rc=$?
    ;;

  *)
    rc=$RC_FAILURE
    echo "[$CMD] Unsupported phase <$phase>; exiting <$rc>."
esac

exit $rc
