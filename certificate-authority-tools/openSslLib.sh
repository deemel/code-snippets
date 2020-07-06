#!/bin/bash

# Underlying CA implementation is accomplished using OpenSSL, an open source
# toolkit implementing the Secure Sockets Layer (SSL v2/v3) and Transport Layer
# Security (TLS v1) protocols as well as a full-strength general purpose
# cryptography library.  This script is sourced in by other scripts (e.g.,
# openSslSetup.sh, mfddCaTool.sh, sampleCertificateCreate.sh) to create
# certificates.

source /opt/mfdd/latest/ca/conf/caConfig.sh

RC_SUCCESS=0
RC_FAILURE=1

DIR_PKI="/etc/pki"
DIR_CA_ORIG="$DIR_PKI/CA"
DIR_CA="$DIR_CA_ORIG/mfdd"
DIR_CA_CERTS="$DIR_CA/certs"
DIR_CA_CRL="$DIR_CA/crl"
DIR_CA_NEWCERTS="$DIR_CA/newcerts"
DIR_CA_PRIVATE="$DIR_CA/private"
DIR_TLS="$DIR_PKI/tls"
FILE_INDEX="$DIR_CA/index.txt"
FILE_SERIAL="$DIR_CA/serial"
FILE_OPENSSL_CONF_TLS="$DIR_TLS/openssl.cnf"
FILE_OPENSSL_CONF="$DIR_CA/openssl.cnf"
PERMISSIONS_DIR_CA_PRIVATE=700 # u+rwx
PERMISSIONS_KEY=400            # u+r
PERMISSIONS_CERTIFICATE=444    # a+r
SERIAL_NUM_INIT=1000

KEY_NUM_BITS=4096
CERTIFICATE_EXTENSION_CA="v3_ca"
CERTIFICATE_MD="sha1"

STR_CA="_CA"
STR_NONE="_NONE"

MFDD_PASS_PHRASE="mfddPass"
MFDD_ROOT_KEY="$DIR_CA_PRIVATE/mfdd.ca-root.key.pem"
MFDD_ROOT_CERTIFICATE="$DIR_CA_CERTS/mfdd.ca-root.cert.pem"
MFDD_INTERMEDIATE_KEY="$DIR_CA_PRIVATE/mfdd.ca-intermediate.key.pem"
MFDD_INTERMEDIATE_CERTIFICATE="$DIR_CA_CERTS/mfdd.ca-intermediate.cert.pem"
MFDD_INTERMEDIATE_CSR="$DIR_CA_CERTS/mfdd.ca-intermediate.csr.pem"
MFDD_CHAIN_CERTIFICATE="$DIR_CA_CERTS/mfdd.ca-chain.cert.pem"

#
# Creates RSA 4096-bit private key.  If pass phrase is "_NONE", then
# key will be unencrypted, else key will be AES 256-bit encrypted with
# specified pass phrase.  Created key will be user readable.
#
function keyCreate {
  passPhrase="$1" # key's pass phrase
  key="$2"        # output filename of key

  if [ "$passPhrase" = "$STR_NONE" ]
  then
    encryptionParams="" # no key encryption
  else
    encryptionParams="-aes256 -passout pass:$passPhrase" # key encryption
  fi

  openssl genrsa $encryptionParams -out $key $KEY_NUM_BITS
  rc=$?

  if [ $rc -eq $RC_SUCCESS ]
  then
    chmod $PERMISSIONS_KEY $key
    rc=$?
  fi

  return $rc
}

#
# Creates certificate or CSR.  If key does not exist, it will be
# created.  If pass phrase is "_NONE", then specified key does not
# require pass phrase.  If isCA is "_CA", then self signed root CA
# certificate will be created and will be valid for specified number
# of days.  For CA certificate creation, country, state, organization,
# and common name must be provided (i.e., non-empty strings); all
# other subject names may be omitted (i.e., empty strings encoded as
# "_NONE").  For non-CA certificate creation, common name must be
# provided (i.e., non-empty string); all other subject names may be
# omitted (i.e., empty strings encoded as "_NONE").  Created
# certificate or CSR will be world readable.
#
function opensslReq {
  passPhrase="$1"         # key's pass phrase
  key="$2"                # certificate or CSR's key
  daysValid="$3"          # number of days certificate is valid
  country="$4"            # subject name, country
  state="$5"              # subject name, state
  locality="$6"           # subject name, locality
  organization="$7"       # subject name, organization
  organizationalUnit="$8" # subject name, organizational unit
  commonName="$9"         # subject name, common name
  emailAddress="${10}"    # subject name, e-mail address
  output="${11}"          # output filename of certificate or CSR
  isCa="${12}"            # set to "_CA" if CA certificate is being created

  if [ ! -f "$key" ]
  then
    keyCreate $passPhrase $key
    rc=$?

    if [ $rc -ne $RC_SUCCESS ]
    then
      return $rc
    fi
  fi

  if [ "$passPhrase" = "$STR_NONE" ]
  then
    passPhraseParams="" # no pass phrase
  else
    passPhraseParams="-passin pass:$passPhrase" # pass phrase
  fi

  if [ "$isCa" = "$STR_CA" ]
  then
    caParams="-x509 -extensions $CERTIFICATE_EXTENSION_CA -days $daysValid" # for CA certificate creation
  else
    caParams="" # for CSR creation
  fi

  subjParamValue=""
  if [ "$country" != "$STR_NONE" ]
  then
    subjParamValue="$subjParamValue/C=$country"
  fi
  if [ "$state" != "$STR_NONE" ]
  then
    subjParamValue="$subjParamValue/ST=$state"
  fi
  if [ "$locality" != "$STR_NONE" ]
  then
    subjParamValue="$subjParamValue/L=$locality"
  fi
  if [ "$organization" != "$STR_NONE" ]
  then
    subjParamValue="$subjParamValue/O=$organization"
  fi
  if [ "$organizationalUnit" != "$STR_NONE" ]
  then
    subjParamValue="$subjParamValue/OU=$organizationalUnit"
  fi
  if [ "$commonName" != "$STR_NONE" ]
  then
    subjParamValue="$subjParamValue/CN=$commonName"
  fi
  if [ "$emailAddress" != "$STR_NONE" ]
  then
    subjParamValue="$subjParamValue/emailAddress=$emailAddress"
  fi

  cmd="openssl req -config $FILE_OPENSSL_CONF -batch -new $caParams $passPhraseParams -key $key -out $output"
  if [ "$subjParamValue" = "" ]
  then
    $cmd
    rc=$?
  else
    subjParamValue="$subjParamValue/"
    $cmd -subj "$subjParamValue"
    rc=$?
  fi

  if [ $rc -eq $RC_SUCCESS ]
  then
    chmod $PERMISSIONS_CERTIFICATE $output
    rc=$?
  fi

  return $rc
}

#
# Creates self signed root CA certificate.
#
function caCreate {
  opensslReq "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}" "${11}" $STR_CA
  rc=$?

  return $rc
}

#
# Creates CSR.
#
function csrCreate {
  opensslReq "$1" "$2" $STR_NONE "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}"
  rc=$?

  return $rc
}

#
# Signs CSR to create signed certificate.  If pass phrase is "_NONE",
# then specified key does not require pass phrase.  If isCA is "_CA",
# then signed CA certificate will be created.  Signed certificate will
# be valid for specified number of days.  SHA1 MD will be used.
# Created signed certificate will be world readable.
#
function csrSign {
  passPhrase="$1" # key's pass phrase
  key="$2"        # CA certificate's key to sign CSR
  ca="$3"         # CA certificate
  daysValid="$4"  # number of days certificate is valid
  csr="$5"        # CSR
  output="$6"     # output filename of signed certificate
  isCa="$7"       # set to "_CA" if CA certificate is being created

  if [ "$passPhrase" = "$STR_NONE" ]
  then
    passPhraseParams="" # no pass phrase
  else
    passPhraseParams="-passin pass:$passPhrase" # pass phrase
  fi

  if [ "$isCa" = "$STR_CA" ]
  then
    caParams="-extensions $CERTIFICATE_EXTENSION_CA" # for CA certificate creation
  else
    caParams="-policy policy_anything" # for non-CA certificate creation
  fi

  openssl ca -config $FILE_OPENSSL_CONF -batch $caParams -days $daysValid -notext -md $CERTIFICATE_MD $passPhraseParams -keyfile $key -cert $ca -in $csr -out $output
  rc=$?

  if [ $rc -eq $RC_SUCCESS ]
  then
    chmod $PERMISSIONS_CERTIFICATE $output
    rc=$?
  fi

  return $rc
}

#
# Signs CSR with MFDD's intermediate CA certificate to create signed
# certificate.
#
function csrSignWithMfddCa {
  csrSign $MFDD_PASS_PHRASE $MFDD_INTERMEDIATE_KEY $MFDD_INTERMEDIATE_CERTIFICATE $1 $2 $3 $4
  rc=$?

  return $rc
}

#
# Verifies certificate chain.  Specified parent and child certificates
# will be verified.
#
function certificateVerify {
  certificateParent="$1" # parent certificate
  certificateChild="$2"  # child certificate

  openssl verify -CAfile $certificateParent $certificateChild
  rc=$?

  return $rc
}

#
# Verifies certificate chain.  MFDD's CA chain certificate (i.e., MFDD's
# root and intermediate CA certificates) and specified child certificate
# will be verified.
#
function certificateVerifyWithMfddCa {
  certificateVerify $MFDD_CHAIN_CERTIFICATE $1
  rc=$?

  return $rc
}

#
# Creates certificate chain.  Created certificate chain will be world
# readable.
#
function certificateChainCreate {
  certificateParent="$1" # parent certificate
  certificateChild="$2"  # child certificate
  chain="$3"

  cat $certificateChild $certificateParent > $chain
  rc=$?

  if [ $rc -eq $RC_SUCCESS ]
  then
    chmod $PERMISSIONS_CERTIFICATE $chain
    rc=$?
  fi

  return $rc
}

#
# Syncs /etc/pki/CA/mfdd with other CA servers.  User may be prompted
# for password in order to access other CA servers.
#
function caServersSync {
  caServers="$1" # comma-separated CA server hostnames

  if [ "$caServers" = "" ]
  then
    caServers="$CA_SERVERS"
  fi

  userId=`whoami`
  #hostnameThis=`hostname --fqdn`
  #ipAddressThis=`host $hostnameThis | cut -d' ' -f4`
  hostnameThis=$(resolve OAM) || return 1
  ipAddressThis=$(getent ahosts ${hostnameThis} | grep STREAM | awk '{print $1}' | sed -n 1p)
  [[ -z "${ipAddressThis}" ]] && return 1

  caServers=${caServers//,/ }
  for i in $caServers
  do
    #ipAddress=`host $i | cut -d' ' -f4`
    ipAddress=$(getent ahosts ${i} | grep STREAM | awk '{print $1}' | sed -n 1p)
    if [ "$ipAddress" = "$ipAddressThis" ]
    then
      continue
    fi

    rsync -avzhe ssh $DIR_CA $userId@$i:$DIR_CA_ORIG
    rc=$?

    if [ $rc -ne $RC_SUCCESS ]
    then
      return $rc
    fi
  done

  return $rc
}
