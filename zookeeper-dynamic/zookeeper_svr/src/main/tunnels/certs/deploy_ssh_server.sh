#!/bin/sh
# Deploy SSH Certificates for SSH Tunnel Server Side

LOG=${dir.tunnels.log}/deploy_ssh_server.$USER.log

{
  TARDIR=/opt/mfdd/tmp/SSH_DST
  # SVRUSR=zookeeper
  # SVRDIR=/home/$SVRUSR

  if [ $# != 1 ]
  then
    echo "Usage: deploy_ssh_server.sh <Tunnel Receiver User>"
    echo "e.g.: deploy_ssh_server.sh tunnel"
    exit -1
  fi

  # Set Server User and dir
  echo "Setting Server User to $1"
  SVRUSR=$1
  SVRDIR=/home/$SVRUSR

  echo "Deploying SSH keys and certificates for server side"
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
  # Server Side
  #
  echo "Deploying SSH host keys and certificate"
  cp -f $TARDIR/ssh_host* /etc/ssh
  if [ $? != 0 ]
  then
    echo "Unsuccessful"
    echo "Exiting...."
    exit -1
  else
    echo "Successful ========================="
  fi
  #
  echo "Deploying server CA pub to $SVRUSR user"
  getent passwd $SVRUSR > /dev/null
  if [ $? != 0 ]
  then
    echo "$SVRUSR user does not exist"
    echo "Exiting...."
    exit -1
  else
    echo "$SVRUSR exists ================="
  fi

  mkdir -p $SVRDIR/.ssh
  echo -n "cert-authority " > $SVRDIR/.ssh/authorized_keys
  cat $TARDIR/ca_key.pub >> $SVRDIR/.ssh/authorized_keys
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
  echo "Changing ownership to $SVRUSR"
  chown -R $SVRUSR:$SVRUSR $SVRDIR
  if [ $? != 0 ]
  then
    echo "Unsuccessful"
    echo "Exiting...."
    exit -1
  else
    echo "Successful ========================="
  fi
  #
  # Update sshd_config
  #
  grep HostCertificate /etc/ssh/sshd_config > /dev/null # returns 0 if found
  if [ $? == 1 ]
  then
    echo "Update sshd_config"
    sed -e "/#Port 22/i\\
HostCertificate /etc/ssh/ssh_host_rsa_key-cert.pub\\
" < /etc/ssh/sshd_config > /etc/ssh/sshd_config.new

    cat >> /etc/ssh/sshd_config.new <<EOF

Match Group tunnel
	AllowTcpForwarding yes
EOF
    mv /etc/ssh/sshd_config.new /etc/ssh/sshd_config
    if [ $? != 0 ]
    then
      echo "Unsuccessful"
      echo "Exiting...."
      exit -1
    else
      echo "Successful ========================="
    fi
  else
    echo "sshd_config already updated"
  fi
  #
  echo "Restart sshd"
  service sshd restart
  if [ $? != 0 ]
  then
    echo "Unsuccessful"
    echo "Exiting...."
    exit -1
  else
    echo "Successful ========================="
  fi
  # TR HW94161
  chmod 600 /etc/ssh/sshd_config
  echo "sshd_config pemission changed to 600 >> " $(ls -lrt /etc/ssh/sshd_config)

# Tee Output to log file
} 2>&1| tee $LOG

exit 0
