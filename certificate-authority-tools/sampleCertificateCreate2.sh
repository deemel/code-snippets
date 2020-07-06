#!/bin/bash

# Similar to sampleCertificateCreate.sh, this sample script creates a sample
# certificate using OpenSSL.  Unlike sampleCertificateCreate.sh, it uses the
# mfddCaTool.sh script instead of directly calling the various functions
# defined in openSslLib.sh.  It is assumed that OpenSSL setup via
# openSslSetup.sh has been run and a CA certificate exists.

source /opt/mfdd/latest/ca/bin/openSslLib.sh

CA_BIN="/opt/mfdd/latest/ca/bin"
export PATH="$PATH:$CA_BIN"

CERTIFICATE_DAYS_VALID=3650
CERTIFICATE_COUNTRY="??" # must be 2 characters
CERTIFICATE_STATE="Sample2 state"
CERTIFICATE_LOCALITY="Sample2 locality"
CERTIFICATE_ORGANIZATION="Sample2 organization"
CERTIFICATE_ORGANIZATIONAL_UNIT="Sample2 organizational unit"
CERTIFICATE_COMMON_NAME="Sample2 common name" # required, must be unique
CERTIFICATE_EMAIL_ADDRESS="Sample2 email address"

KEY="$DIR_CA_PRIVATE/sample2.key.pem"
CSR="$DIR_CA_CERTS/sample2.csr.pem"
CERTIFICATE="$DIR_CA_CERTS/sample2.cert.pem"

######################################################################
# Creates sample certificate using OpenSSL via mfddCaTool.sh in MFDD
# environment.
######################################################################

#
# Create key with no pass phrase (encoded as "_NONE").
#
mfddCaTool.sh keyCreate $STR_NONE $KEY
rc=$?
if [ $rc -ne $RC_SUCCESS ]
then
  echo "Problem encountered in creating certificate's key."
  exit $rc
fi

#
# Create CSR.
#
mfddCaTool.sh csrCreate $STR_NONE $KEY "$CERTIFICATE_COUNTRY" "$CERTIFICATE_STATE" "$CERTIFICATE_LOCALITY" "$CERTIFICATE_ORGANIZATION" "$CERTIFICATE_ORGANIZATIONAL_UNIT" "$CERTIFICATE_COMMON_NAME" "$CERTIFICATE_EMAIL_ADDRESS" $CSR
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
mfddCaTool.sh csrSignWithMfddCa $CERTIFICATE_DAYS_VALID $CSR $CERTIFICATE
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
mfddCaTool.sh certificateVerifyWithMfddCa $CERTIFICATE
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
mfddCaTool.sh caServersSync
rc=$?
if [ $rc -ne $RC_SUCCESS ]
then
  echo "Problem encountered in syncing CA servers."
  exit $rc
fi

exit $RC_SUCCESS
