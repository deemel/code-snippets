#!/bin/bash

# OpenSSL setup script.  It uses the various functions defined in
# openSslLib.sh.  It is invoked during installation to configure OpenSSL.

# When invoked with no options, it sets up OpenSSL.  Setup is intended to be
# only performed once during installation.  Any existing CA data (e.g., keys,
# certificates) will be removed before setup.  CA directories, index file,
# serial number file, OpenSSL configuration will be created.  All CA data
# (e.g., keys, certificates) should reside in /etc/pki/CA/mfdd.

# Uninstall removes /etc/pki/CA/mfdd.

source /opt/mfdd/latest/ca/bin/openSslLib.sh

RC_SUCCESS=0
RC_FAILURE=1
STR_SUCCESS="success"
STR_FAILURE="failure"
ARGS_MIN=0
ARGS_MAX=1
ARGS_ONE=1
USER_ID="root"
PROFILE="$HOME/.bash_profile"

PARAM_HELP="-h"
PARAM_UNINSTALL="uninstall"
PARAM_CREATE_ROOT_INTERMEDIATE_CA="createRootIntermediateCa"

STR_QUIET="_QUIET"

command=`basename $0`
pid=$$

#
# Displays script usage.
#
function displayUsage {
  rc=$1

  echo
  echo "USAGE: $command [$PARAM_UNINSTALL | $PARAM_CREATE_ROOT_INTERMEDIATE_CA]"
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

#
# Gets current date and time (e.g., "2014/02/26 17:31:17").
#
function getTimestamp {
  echo `date +"%Y/%m/%d %H:%M:%S"`
}

#
# Displays script start message with timestamp.
#
function scriptStart {
  timestamp=`getTimestamp`

  echo
  echo "----------------------------------------------------------------------"
  echo "MFDD OpenSSL setup script invoked on $timestamp."
  echo "----------------------------------------------------------------------"
  echo
}

#
# Displays script end message with timestamp and status.
#
function scriptEnd {
  rc=$1

  if [ $rc -ne $RC_SUCCESS ]
  then
    status="$STR_FAILURE"
  else
    status="$STR_SUCCESS"
  fi

  timestamp=`getTimestamp`

  echo
  echo "----------------------------------------------------------------------"
  echo "MFDD OpenSSL setup script completed on $timestamp."
  echo "Status = $status"
  echo "----------------------------------------------------------------------"
  echo

  exit $rc
}

#
# Performs uninstallation.  Removes /etc/pki/CA/mfdd.
#
function uninstall {
  isQuiet="$1"

  if [ "$isQuiet" != "$STR_QUIET" ]
  then
    echo "Uninstalling MFDD OpenSSL setup."
    echo "Deleting files in <$DIR_CA>."
  fi

  /bin/rm -rf $DIR_CA 2> /dev/null

  if [ "$isQuiet" != "$STR_QUIET" ]
  then
    scriptEnd $RC_SUCCESS
  fi
}

#
# Creates root and intermediate CA certificates.  Both CA
# certificates' keys share same pass phrase (sourced in from
# /opt/mfdd/latest/ca/bin/openSslLib.sh).  Both CA certificates will
# be valid for specified number of days.  Values for subject fields
# are provided during installation time and sourced in from
# /opt/mfdd/latest/ca/conf/caConfig.sh.  CA chain certificate will be
# created, containing both root and intermediate CA certificates. 
#
function createRootIntermediateCa {
  #
  # - Create root CA certificate's key.
  # - Create root CA certificate.
  #

  echo "Creating root CA certificate's key."
  keyCreate $MFDD_PASS_PHRASE $MFDD_ROOT_KEY
  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    return $rc
  fi

  echo "Creating root CA certificate."
  caCreate $MFDD_PASS_PHRASE $MFDD_ROOT_KEY $CERTIFICATE_DAYS_VALID "$CERTIFICATE_COUNTRY" "$CERTIFICATE_STATE" "$CERTIFICATE_LOCALITY" "$CERTIFICATE_ORGANIZATION" "$CERTIFICATE_ORGANIZATIONAL_UNIT" "$CERTIFICATE_COMMON_NAME" "$CERTIFICATE_EMAIL_ADDRESS" $MFDD_ROOT_CERTIFICATE $STR_CA
  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    return $rc
  fi

  #
  # - Create intermediate CA certificate's key.
  # - Create intermediate CA CSR.
  # - Sign intermediate CA CSR with root CA certificate to create
  #   intermediate CA certificate.
  # - Verify intermediate CA certificate.
  # - Create root and intermediate chain certificate.
  #

  echo "Creating intermediate CA certificate's key."
  keyCreate $MFDD_PASS_PHRASE $MFDD_INTERMEDIATE_KEY
  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    return $rc
  fi

  echo "Creating intermediate CA CSR."
  csrCreate $MFDD_PASS_PHRASE $MFDD_INTERMEDIATE_KEY "$CERTIFICATE_COUNTRY" "$CERTIFICATE_STATE" "$CERTIFICATE_LOCALITY" "$CERTIFICATE_ORGANIZATION" "$CERTIFICATE_ORGANIZATIONAL_UNIT" "$CERTIFICATE_COMMON_NAME" "$CERTIFICATE_EMAIL_ADDRESS" $MFDD_INTERMEDIATE_CSR
  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    return $rc
  fi

  echo "Signing intermediate CA CSR with root CA certificate to create intermediate CA certificate."
  csrSign $MFDD_PASS_PHRASE $MFDD_ROOT_KEY $MFDD_ROOT_CERTIFICATE $CERTIFICATE_DAYS_VALID $MFDD_INTERMEDIATE_CSR $MFDD_INTERMEDIATE_CERTIFICATE $STR_CA
  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    return $rc
  fi

  echo "Verifying intermediate CA certificate."
  certificateVerify $MFDD_ROOT_CERTIFICATE $MFDD_INTERMEDIATE_CERTIFICATE
  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    return $rc
  fi

  echo "Creating root and intermediate CA chain certificate."
  certificateChainCreate $MFDD_ROOT_CERTIFICATE $MFDD_INTERMEDIATE_CERTIFICATE $MFDD_CHAIN_CERTIFICATE
  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    return $rc
  fi

  return $RC_SUCCESS
}

#
# Sets up OpenSSL for MFDD.  Setup is intended to be only performed
# once during MFDD installation.  Any existing MFDD CA data (e.g.,
# keys, certificates) will be removed before setup.  CA
# directories, index file, serial number file, OpenSSL configuration
# will be created.  All MFDD CA data (e.g., keys, certificates)
# should reside in /etc/pki/CA/mfdd.
#
function openSslSetup {
  uninstall $STR_QUIET

  echo "Setting up MFDD OpenSSL."

  echo "Creating directories and files in <$DIR_CA>."  
  for i in $DIR_CA_CERTS $DIR_CA_CRL $DIR_CA_NEWCERTS $DIR_CA_PRIVATE
  do
    mkdir -p $i
  done

  chmod $PERMISSIONS_DIR_CA_PRIVATE $DIR_CA_PRIVATE
  touch $FILE_INDEX
  echo $SERIAL_NUM_INIT > $FILE_SERIAL

  if [ ! -f "$FILE_OPENSSL_CONF_TLS" ]
  then
    echo "File <$FILE_OPENSSL_CONF_TLS> does not exist; exiting."
    uninstall $STR_QUIET
    return $RC_FAILURE
  fi

  cp -p $FILE_OPENSSL_CONF_TLS $FILE_OPENSSL_CONF
  sed -i "s#$DIR_CA_ORIG#$DIR_CA#g" $FILE_OPENSSL_CONF
  rc=$?

  return $rc
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

  if [ "$1" = "$PARAM_UNINSTALL" ]
  then
    scriptStart
    uninstall
  fi

  if [ "$1" != "$PARAM_CREATE_ROOT_INTERMEDIATE_CA" ]
  then
    displayUsage $RC_FAILURE
  fi
fi

scriptStart

openSslSetup
rc=$?
if [ $rc -ne $RC_SUCCESS ]
then
  scriptEnd $rc
fi

if [ "$1" = "$PARAM_CREATE_ROOT_INTERMEDIATE_CA" ]
then
  createRootIntermediateCa
  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    scriptEnd $rc
  fi
fi

scriptEnd $RC_SUCCESS
