${rpm.scriptlet.shebang}

${rpm.scriptlet.debug}

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

USER_ZOOKEEPER="${user.zookeeper}"
USER_TUNNEL="${user.tunnel}"
GROUP_ZOOKEEPER="${group.zookeeper}"
GROUP_TUNNEL="${group.tunnel}"

RC_SUCCESS=${rc.success}
LOG_DATE_FORMAT="${log.date.format}"

DIR_HOME_TUNNEL="${dir.home.tunnel}"
DIR_HOME_ZOOKEEPER="${dir.home.zookeeper}"

# ======================================================================
# Constants.
# ======================================================================

LOGIN_SHELL_NONE="/bin/false"

# ======================================================================

${rpm.scriptlet.cmd.runtime}
${rpm.scriptlet.cmd.preinstall}
${rpm.scriptlet.cmd.short}
${rpm.scriptlet.cmd.log}

#
# Display usage.  More info at
# https://fedoraproject.org/wiki/Packaging:Scriptlets
#
function displayUsage() {
  local func="${FUNCNAME[0]}"

  echo "USAGE: $cmd {1|2}"
  echo "         1: install"
  echo "         2: upgrade"

  ${rpm.scriptlet.exit}
}

{
  ${rpm.scriptlet.check.num.args}

  if [ $1 -ge 2 ]
  then
    ${rpm.scriptlet.cmd.upgrade}
  fi

  ${rpm.scriptlet.echo.started}

  if [ $1 -ge 2 ]
  then
    ${cmd.source.zk.mfdd.lib}

    zkCrontabRemove

    #
    # Get init process.  Stop ZK service.
    #
    initProcess=""
    initProcessGet initProcess

    case "$initProcess" in
      "$INIT_PROCESS_SYSVINIT" | "$INIT_PROCESS_UPSTART")
        zkServiceTearDownSysVinit $STR_TRUE
        ;;

      "$INIT_PROCESS_SYSTEMD")
        zkServiceTearDownSystemd $STR_TRUE
        ;;

      *)
        rc=$RC_SUCCESS

        echo2 "[$cmd] Unsupported init process <$initProcess>; exiting <$rc>."
        exit $rc
    esac
  else
    #
    # Get tunnels group account.
    #
    getent group $GROUP_TUNNEL > /dev/null
    if [ $? -ne $RC_SUCCESS ]
    then
      cmdToCall="groupadd --system $GROUP_TUNNEL"
      timestamp=$(date +"$LOG_DATE_FORMAT")

      echo
      echo "$timestamp [$cmd] Running command <$cmdToCall>."

      #
      # Create tunnels system group.
      #
      $cmdToCall
    fi

    #
    # Get tunnels user account.
    #
    getent passwd $USER_TUNNEL > /dev/null
    if [ $? -ne $RC_SUCCESS ]
    then
      cmdToCall="useradd --create-home --home-dir $DIR_HOME_TUNNEL --shell $LOGIN_SHELL_NONE --groups $GROUP_TUNNEL $USER_TUNNEL"
      timestamp=$(date +"$LOG_DATE_FORMAT")

      echo
      echo "$timestamp [$cmd] Running command <$cmdToCall>."

      #
      # Create tunnels user account with no login shell.
      #
      $cmdToCall
    fi

    cmdToCall="chage -I -1 -m 0 -M 99999 -E -1 $USER_TUNNEL"
    timestamp=$(date +"$LOG_DATE_FORMAT")

    echo
    echo "$timestamp [$cmd] Running command <$cmdToCall>."

    #
    # Change tunnels user account to:
    #   - -I -1: remove inactivity
    #   - -m 0: change password at any time
    #   - -M 99999: have long password validity
    #   - -E -1: remove expiration date
    #
    $cmdToCall

    CREATE_HOME_ARG="--create-home"
    if [ -d $DIR_HOME_ZOOKEEPER ]
    then
      CREATE_HOME_ARG=""
    fi

    #
    # Get ZK user account.
    #
    zkAccount=$(getent passwd $USER_ZOOKEEPER)
    if [ $? -ne $RC_SUCCESS ]
    then
      cmdToCall="useradd --system $CREATE_HOME_ARG --home-dir $DIR_HOME_ZOOKEEPER --shell $LOGIN_SHELL_NONE $USER_ZOOKEEPER"
      timestamp=$(date +"$LOG_DATE_FORMAT")

      echo
      echo "$timestamp [$cmd] Running command <$cmdToCall>."

      #
      # Create ZK system account with no login shell.
      # Do create a home directory because this is where certs will live.
      #
      $cmdToCall
    fi

    #
    # Make sure pre-existing zookeeper home dir is owned by
    # zookeeper:zookeeper.  Can happen after an rpm -e then reinstall.
    #
    if [ -z $CREATE_HOME_ARG ]
    then
      cmdToCall="chown -R $USER_ZOOKEEPER:$GROUP_ZOOKEEPER $DIR_HOME_ZOOKEEPER"
      timestamp=$(date +"$LOG_DATE_FORMAT")

      echo
      echo "$timestamp [$cmd] Running command <$cmdToCall>."

      $cmdToCall
    fi

    #
    # Modify already existing ZK user account to have no login shell if
    # necessary.
    #
    if [ ! -z "$zkAccount" ]
    then
      loginShell=$(echo "$zkAccount" | cut -d':' -f7)
      if [ "$loginShell" != "$LOGIN_SHELL_NONE" ]
      then
        cmdToCall="usermod --shell $LOGIN_SHELL_NONE $USER_ZOOKEEPER"
        timestamp=$(date +"$LOG_DATE_FORMAT")

        echo
        echo "$timestamp [$cmd] Running command <$cmdToCall>."

        $cmdToCall
      fi
    fi
  fi

  ${rpm.scriptlet.echo.completed}

  rc=$?
  exit $rc
} 2>&1 | tee $log

#
# RPM scriptlets MUST exit with 0 (i.e., success) exit code.  More info at
# https://fedoraproject.org/wiki/Packaging:Scriptlets.
#
${rpm.scriptlet.exit}
