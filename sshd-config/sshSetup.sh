#!/bin/bash

RC_SUCCESS=0
RC_FAILURE=1
MIN_ARGS=0
MAX_ARGS=1
PROFILE="$HOME/.bash_profile"

PARAM_HELP="-h"
PARAM_UNINSTALL="uninstall"

USER="root"
FILE_SSHD_CONFIG="/etc/ssh/sshd_config"
SSH_GROUP="sshusers"
#SSH_SSHD_MATCH_GROUP="Match Group $SSH_GROUP"

command=`basename $0`
pid=$$

function displayUsage {
  echo
  echo "USAGE: $command [$PARAM_UNINSTALL]"
  echo

  exit $RC_FAILURE
}

function sshdRestart {
  cmp -s $sshdConfigFileNew $FILE_SSHD_CONFIG
  if [ $? -ne $RC_SUCCESS ]
  then
    /bin/chmod --reference=$FILE_SSHD_CONFIG $sshdConfigFileNew
    /bin/mv -f $sshdConfigFileNew $FILE_SSHD_CONFIG

    echo "Restarting sshd."
    service sshd restart
  fi
}

function end {
  timestamp=`date +"%Y/%m/%d %H:%M:%S"`

  echo
  echo "SSH setup script completed on $timestamp."
  echo

  exit $RC_SUCCESS
}

if [ -f "$PROFILE" ]
then
  . $PROFILE
fi

if [ $# -lt $MIN_ARGS ]
then
  displayUsage
fi

if [ $# -gt $MAX_ARGS ]
then
  displayUsage
fi

if [ $# -eq $MAX_ARGS ]
then
  if [ "$1" = "$PARAM_HELP" ]
  then
    displayUsage
  fi

  if [ "$1" != "$PARAM_UNINSTALL" ]
  then
    displayUsage
  fi
fi

i=`whoami`
if [ "$i" != "$USER" ]
then
  echo "Must be user <$USER> to run this script; exiting."
  exit $RC_FAILURE
fi

timestamp=`date +"%Y/%m/%d %H:%M:%S"`

echo
echo "----------------------------------------------------------------------"
echo "MFDD SSH setup script invoked on $timestamp."
echo "----------------------------------------------------------------------"
echo

sshdConfigFilename=`echo $FILE_SSHD_CONFIG | cut -d"/" -f4`
sshdConfigFileBackup="/tmp/$sshdConfigFilename.$pid.old"
sshdConfigFileNew="/tmp/$sshdConfigFilename.$pid.new"

setupAllowGroups="AllowGroups root mfdd ${SSH_GROUP}"

echo "Backing up file <$FILE_SSHD_CONFIG> to <$sshdConfigFileBackup>."
/bin/cp -f $FILE_SSHD_CONFIG $sshdConfigFileBackup

if [ "$1" = "$PARAM_UNINSTALL" ]
then
  echo "Uninstalling MFDD SSH setup."

  echo "Deleting group <$SSH_GROUP>."
  getent group $SSH_GROUP > /dev/null && groupdel $SSH_GROUP 2> /dev/null

  echo "Unconfiguring sshd if necessary."
  sed -e "/${setupAllowGroups}/d" $FILE_SSHD_CONFIG > $sshdConfigFileNew

  sshdRestart
  end
fi

echo "Adding group <$SSH_GROUP>."
getent group $SSH_GROUP 2> /dev/null || groupadd -r $SSH_GROUP

# special access for user cfuser 
getent passwd cfuser 2> /dev/null && usermod -a -G $SSH_GROUP cfuser 

echo "Configuring sshd's SSH AllowGroups if necessary."
/bin/cp -f $FILE_SSHD_CONFIG $sshdConfigFileNew
i=`grep -c "${setupAllowGroups}"  $sshdConfigFileNew`
if [ $i -lt 1 ]
then
  echo "Configuring sshd's SSH AllowGroups groups"
  sed -e "/AllowAgentForwarding/i${setupAllowGroups}" $FILE_SSHD_CONFIG > $sshdConfigFileNew
fi

echo "Turning on SELinux ssh_chroot_full_access."
setsebool -P ssh_chroot_full_access on

sshdRestart
end
