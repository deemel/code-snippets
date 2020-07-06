#!/bin/sh
# Deploy SSH Certificates for SSH Tunnel Client Side

LOG=${dir.tunnels.log}/deploy_ssh_client.$USER.log

{
  TARDIR=/opt/mfdd/tmp/SSH_DST
  # USRDIR=/home/tunnel

  if [ $# != 1 ]
  then
    echo "Usage: deploy_ssh_client.sh <SSH Tunnel User>"
    echo "e.g.: deploy_ssh_client.sh tunnel"
    exit -1
  fi

  # Set Tunnel User and dir
  echo "Setting Tunnel User to $1"
  TUNUSR=$1
  USRDIR=/home/$TUNUSR

  echo "untarring keys and certificates"
  if [ ! -d $TARDIR ]
  then
    echo "SSH Certificate TAR Directory does not exist"
    echo "Exiting...."
    exit -1
  fi

  cd $TARDIR
  echo "Untarring SSH Certificates"
  tar -xvf CA.tar
  if [ $? != 0 ]
  then
    echo "Unsuccessful"
    echo "Exiting...."
    exit -1
  else
    echo "Successful ========================="
  fi
  #
  # Client Side
  #
  echo "Deploy client keys and cert"
  if [ ! -d $USRDIR ]
  then
    echo "Tunnel User Home Directory does not exist!"
    echo "Exiting...."
    exit -1
  fi
  mkdir -p $USRDIR/.ssh
  chmod 700 $USRDIR/.ssh
  echo "Deploy client certificate"
  cp $TARDIR/id_rsa* $USRDIR/.ssh
  if [ $? != 0 ]
  then
    echo "Unsuccessful"
    echo "Exiting...."
    exit -1
  else
    echo "Successful ========================="
  fi
  echo "Deploy CA certificate for client side"
  echo -n "@cert-authority * " > $USRDIR/.ssh/known_hosts
  cat $TARDIR/ca_key.pub >> $USRDIR/.ssh/known_hosts
  if [ $? != 0 ]
  then
    echo "Unsuccessful"
    echo "Exiting...."
    exit -1
  else
    echo "Successful ========================="
  fi
  #
  # Change ownership of files
  #
  echo "Changing ownership to $TUNUSR"
  chown -R $TUNUSR:$TUNUSR $USRDIR
  if [ $? != 0 ]
  then
    echo "Unsuccessful"
    echo "Exiting...."
    exit -1
  else
    echo "Successful ========================="
  fi

# Tee Output to log file
} 2>&1| tee $LOG

exit 0
