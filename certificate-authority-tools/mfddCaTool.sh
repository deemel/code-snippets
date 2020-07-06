#!/bin/bash

# Instead of sourcing in openSslLib.sh, this script can be called by other
# scripts (e.g., sampleCertificateCreate2.sh) to create certificates.  It is a
# wrapper of most of the functions defined in openSslLib.sh.

source /opt/mfdd/latest/ca/bin/openSslLib.sh

RC_SUCCESS=0
RC_FAILURE=1
ARGS_MIN=1
ARGS_MAX=11
ARGS_ONE=1
USER_ID="root"
PROFILE="$HOME/.bash_profile"

PARAM_HELP="-h"
PARAM_KEY_CREATE="keyCreate"
PARAM_CSR_CREATE="csrCreate"
PARAM_CSR_SIGN="csrSign"
PARAM_CSR_SIGN_WITH_MFDD_CA="csrSignWithMfddCa"
PARAM_CERTIFICATE_VERIFY="certificateVerify"
PARAM_CERTIFICATE_VERIFY_WITH_MFDD_CA="certificateVerifyWithMfddCa"
PARAM_CA_SERVERS_SYNC="caServersSync"

command=`basename $0`

#
# Displays script usage.
#
function displayUsage {
  rc=$1

  echo
  echo "USAGE:"
  echo "  $command $PARAM_KEY_CREATE <key's pass phrase> <output filename of key>"
  echo "  $command $PARAM_CSR_CREATE <key's pass phrase> <CSR's key> <subject name, country> <subject name, state> <subject name, locality> <subject name, organization> <subject name, organizational unit> <subject name, common name> <subject name, e-mail address> <output filename of CSR>"
  echo "  $command $PARAM_CSR_SIGN <key's pass phrase> <CA certificate's key to sign CSR> <CA certificate> <number of days certificate is valid> <CSR> <output filename of signed certificate> [$STR_CA]"
  echo "  $command $PARAM_CSR_SIGN_WITH_MFDD_CA <number of days certificate is valid> <CSR> <output filename of signed certificate> [$STR_CA]"
  echo "  $command $PARAM_CERTIFICATE_VERIFY <parent certificate> <child certificate>"
  echo "  $command $PARAM_CERTIFICATE_VERIFY_WITH_MFDD_CA <child certificate>"
  echo "  $command $PARAM_CA_SERVERS_SYNC [<comma-separated CA server hostnames>]"
  echo
  echo "If applicable, following fields may be empty strings, encoded as \"$STR_NONE\"."
  echo "  - <key's pass phrase>"
  echo "  - <subject name, country>"
  echo "  - <subject name, state>"
  echo "  - <subject name, locality>"
  echo "  - <subject name, organization>"
  echo "  - <subject name, organizational unit>"
  echo "  - <subject name, common name>"
  echo "  - <subject name, e-mail address>"
  echo
  echo "\"$STR_CA\" indicates signed CA certificate will be created during signing."
  echo

  exit $rc
}

#
# Validates user of script to be root.
#
function validateUser {
  userId=`whoami`
  if [ "$userId" != "$USER_ID" ]
  then
    echo "User ID is not <$USER_ID>; exiting."
    exit $RC_FAILURE
  fi
}

######################################################################
# main
######################################################################

validateUser

if [ -f "$PROFILE" ]
then
  source $PROFILE
fi

if [ $# -lt $ARGS_MIN ]
then
  displayUsage $RC_FAILURE
fi

if [ $# -gt $ARGS_MAX ]
then
  displayUsage $RC_FAILURE
fi

if [ $# -eq $ARGS_ONE ]
then
  if [ "$1" = "$PARAM_HELP" ]
  then
    displayUsage $RC_SUCCESS
  fi

  if [ "$1" != "$PARAM_CA_SERVERS_SYNC" ]
  then
    displayUsage $RC_FAILURE
  fi
fi

case "$1" in
  "$PARAM_KEY_CREATE")
    keyCreate $2 $3
    rc=$?
    ;;

  "$PARAM_CSR_CREATE")
    csrCreate $2 $3 "$4" "$5" "$6" "$7" "$8" "$9" "${10}" ${11}
    rc=$?
    ;;

  "$PARAM_CSR_SIGN")
    csrSign $2 $3 $4 $5 $6 $7 $8
    rc=$?
    ;;

  "$PARAM_CSR_SIGN_WITH_MFDD_CA")
    csrSignWithMfddCa $2 $3 $4 $5
    rc=$?
    ;;

  "$PARAM_CERTIFICATE_VERIFY")
    certificateVerify $2 $3
    rc=$?
    ;;

  "$PARAM_CERTIFICATE_VERIFY_WITH_MFDD_CA")
    certificateVerifyWithMfddCa $2
    rc=$?
    ;;

  "$PARAM_CA_SERVERS_SYNC")
    caServersSync $2
    rc=$?
    ;;

  *)
    displayUsage $RC_FAILURE
    ;;
esac

exit $rc
