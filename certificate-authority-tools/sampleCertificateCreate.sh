#!/bin/bash

# This creates a sample certificate using OpenSSL.  It uses the various
# functions defined in openSslLib.sh.  It is assumed that OpenSSL setup via
# openSslSetup.sh has been run and a CA certificate exists.  Creating a
# certificate using OpenSSL consists of the following steps.
#
# 1. Create key with pass phrase or no pass phrase (encoded as "_NONE").
# 2. Create CSR.
# 3. Sign CSR with CA certificate (e.g., intermediate CA certificate).
#    Resulting certificate will be valid for specified number of days.
# 4. Verify newly created certificate with CA certificate (e.g., root and
#    intermediate CA chain certificate).
# 5. Sync up CA servers with newly created files (e.g., key, CSR, certificate).
#    You may be prompted for password in order to access other CA servers.

source /opt/mfdd/latest/ca/bin/openSslLib.sh

CERTIFICATE_DAYS_VALID=3650
CERTIFICATE_COUNTRY="??" # must be 2 characters
CERTIFICATE_STATE="Sample state"
CERTIFICATE_LOCALITY="Sample locality"
CERTIFICATE_ORGANIZATION="Sample organization"
CERTIFICATE_ORGANIZATIONAL_UNIT="Sample organizational unit"
CERTIFICATE_COMMON_NAME="Sample common name" # required, must be unique
CERTIFICATE_EMAIL_ADDRESS="Sample email address"

KEY="$DIR_CA_PRIVATE/sample.key.pem"
CSR="$DIR_CA_CERTS/sample.csr.pem"
CERTIFICATE="$DIR_CA_CERTS/sample.cert.pem"

######################################################################
# Creates sample certificate using OpenSSL via openSslLib.sh in MFDD
# environment.
######################################################################

#
# Create key with no pass phrase (encoded as "_NONE").
#
keyCreate $STR_NONE $KEY
rc=$?
if [ $rc -ne $RC_SUCCESS ]
then
  echo "Problem encountered in creating certificate's key."
  exit $rc
fi

#
# Create CSR.
#
csrCreate $STR_NONE $KEY "$CERTIFICATE_COUNTRY" "$CERTIFICATE_STATE" "$CERTIFICATE_LOCALITY" "$CERTIFICATE_ORGANIZATION" "$CERTIFICATE_ORGANIZATIONAL_UNIT" "$CERTIFICATE_COMMON_NAME" "$CERTIFICATE_EMAIL_ADDRESS" $CSR
rc=$?
if [ $rc -ne $RC_SUCCESS ]
then
  echo "Problem encountered in creating CSR."
  exit $rc
fi

#
# Sign CSR with MFDD's intermediate CA certificate.  Resulting
# certificate will be valid for specified 3650 days (i.e., 10 years).
#
csrSignWithMfddCa $CERTIFICATE_DAYS_VALID $CSR $CERTIFICATE
rc=$?
if [ $rc -ne $RC_SUCCESS ]
then
  echo "Problem encountered in signing CSR.  Is common name <$CERTIFICATE_COMMON_NAME> unique?"
  exit $rc
fi

#
# Verify newly created certificate with MFDD's CA chain certificate
# (i.e., MFDD's root and intermediate CA certificates).
#
certificateVerifyWithMfddCa $CERTIFICATE
rc=$?
if [ $rc -ne $RC_SUCCESS ]
then
  echo "Problem encountered in verifying certificate."
  exit $rc
fi

#
# Sync up CA servers with newly created files (e.g., key, CSR,
# certificate).  You may be prompted for password in order to access
# other CA servers.
#
caServersSync
rc=$?
if [ $rc -ne $RC_SUCCESS ]
then
  echo "Problem encountered in syncing CA servers."
  exit $rc
fi

exit $RC_SUCCESS
