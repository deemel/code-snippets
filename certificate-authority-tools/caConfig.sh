#!/bin/bash

# The CA_SERVERS environment variable contains a comma-separated list of
# hostnames that constitute the CA servers.  It is used to sync up CA data
# files (e.g., private keys, CSRs, certificates) among CA servers.
#
# The CERTIFICATE_* environment variables are used during installation time to
# create the root CA certificate as well as the intermediate CA certificate.
# The root and intermediate CA certificates are chained together.  An
# intermediate CA is an intermediary that can sign certificates on behalf of
# the root CA.  A certificate can be signed by the intermediate CA, which
# itself is signed by the root CA, so a chain of trust is formed.

CAHOME=/opt/mfdd/latest/ca
export CABIN=$CAHOME/bin
export CACONF=$CAHOME/conf

export CA_SERVERS="vmx0335.jupiter.com,vmx0638.jupiter.com,vmx0639.jupiter.com,vmx0640.jupiter.com,vmx0641.jupiter.com"
export CERTIFICATE_DAYS_VALID="3650"
export CERTIFICATE_COUNTRY="US"
export CERTIFICATE_STATE="CA"
export CERTIFICATE_LOCALITY="San Francisco"
export CERTIFICATE_ORGANIZATION="Jupiter"
export CERTIFICATE_ORGANIZATIONAL_UNIT="SNI"
export CERTIFICATE_COMMON_NAME="SNI"
export CERTIFICATE_EMAIL_ADDRESS="_NONE"

PATH=$PATH:${CAHOME}/bin
export PATH
