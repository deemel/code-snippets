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
SFTP_GROUP="sftpusers"
SFTP_USER="sftpuser"
SFTP_USER_HOME="/var/log"
SFTP_USER_SHELL="/sbin/nologin"
SFTP_SSHD_MATCH_GROUP="Match Group $SFTP_GROUP"

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
    /bin/mv -f $sshdConfigFileNew $FILE_SSHD_CONFIG

    echo "Restarting sshd."
    service sshd restart
  fi
}

function end {
  timestamp=`date +"%Y/%m/%d %H:%M:%S"`

  echo
  echo "SFTP setup script completed on $timestamp."
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
echo "MFDD SFTP setup script invoked on $timestamp."
echo "----------------------------------------------------------------------"
echo

sshdConfigFilename=`echo $FILE_SSHD_CONFIG | cut -d"/" -f4`
sshdConfigFileBackup="/opt/mfdd/tmp/$sshdConfigFilename.$pid.0"
sshdConfigFileNew="/opt/mfdd/tmp/$sshdConfigFilename.$pid.new"

if [ -e "$FILE_SSHD_CONFIG" ]; then
    echo "Backing up file <$FILE_SSHD_CONFIG> to <$sshdConfigFileBackup>."
    /bin/cp -f $FILE_SSHD_CONFIG $sshdConfigFileBackup
else
    echo "SSHD not installed, nothing to configure."
    end
fi

if [ "$1" = "$PARAM_UNINSTALL" ]
then
  echo "Uninstalling MFDD SFTP setup."

  echo "Deleting user <$SFTP_USER>."
  userdel $SFTP_USER 2> /dev/null
  /bin/rm -f /var/spool/mail/$SFTP_USER 2> /dev/null

  echo "Deleting group <$SFTP_GROUP>."
  groupdel $SFTP_GROUP 2> /dev/null

  echo "Unconfiguring sshd if necessary."
  sed -e "s/#Subsystem\tsftp\t\/usr\/libexec\/openssh\/sftp-server/Subsystem\tsftp\t\/usr\/libexec\/openssh\/sftp-server/g" -e "/Subsystem\tsftp\tinternal-sftp/d" -e "/^$SFTP_SSHD_MATCH_GROUP/,+4d" $FILE_SSHD_CONFIG > $sshdConfigFileNew

  sshdRestart
  end
fi

if ! getent group "${SFTP_GROUP}" > /dev/null; then
  echo "Adding group <$SFTP_GROUP>."
  groupadd $SFTP_GROUP
fi

if ! getent passwd "$SFTP_USER" > /dev/null; then
  echo "Adding user <$SFTP_USER>."
  echo "*** Please manually set password for user <$SFTP_USER> (i.e., \"passwd $SFTP_USER\") before 1st use."
  useradd -g $SFTP_GROUP -d $SFTP_USER_HOME -s $SFTP_USER_SHELL -M $SFTP_USER 2> /dev/null
fi

echo "Configuring sshd's SFTP subsystem if necessary."
sed "s/^Subsystem\tsftp\t\/usr\/libexec\/openssh\/sftp-server/#\0\nSubsystem\tsftp\tinternal-sftp/g" $FILE_SSHD_CONFIG > $sshdConfigFileNew

i=`grep -c "^$SFTP_SSHD_MATCH_GROUP" $sshdConfigFileNew`
if [ $i -lt 1 ]
then
  echo "Configuring sshd's SFTP match group."
  cat <<EOF >> $sshdConfigFileNew

Match Group $SFTP_GROUP
	ChrootDirectory $SFTP_USER_HOME
	X11Forwarding no
	AllowTcpForwarding no
	ForceCommand internal-sftp
EOF
fi

echo "Turning on SELinux ssh_chroot_full_access."
setsebool -P ssh_chroot_full_access on

sshdRestart
end
