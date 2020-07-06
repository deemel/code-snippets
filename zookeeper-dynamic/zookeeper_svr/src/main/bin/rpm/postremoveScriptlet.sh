${rpm.scriptlet.shebang}

${rpm.scriptlet.debug}

# ======================================================================
# Shell variables originating from Maven properties.
# ======================================================================

USER_ZOOKEEPER="${user.zookeeper}"
GROUP_ZOOKEEPER="${group.zookeeper}"
USER_TUNNEL="${user.tunnel}"

LOG_DATE_FORMAT="${log.date.format}"

DIR_ZK_LOG="${dir.zk.log}"
DIR_ZK_ROOT="${dir.zk.root}"
DIR_TUNNELS_LOG="${dir.tunnels.log}"

LINK_ZOOKEEPER="${link.zookeeper}"

FILE_ZK_CONFIG_SH="${file.zk.config.sh}"

# ======================================================================

${rpm.scriptlet.cmd.runtime}
${rpm.scriptlet.cmd.postremove}
${rpm.scriptlet.cmd.short}
${rpm.scriptlet.cmd.log}

#
# Display usage.  More info at
# https://fedoraproject.org/wiki/Packaging:Scriptlets
#
function displayUsage() {
  local func="${FUNCNAME[0]}"

  echo "USAGE: $cmd {0|1}"
  echo "         0: uninstall"
  echo "         1: upgrade"

  ${rpm.scriptlet.exit}
}

{
  ${rpm.scriptlet.check.num.args}

  if [ $1 -ge 1 ]
  then
    ${rpm.scriptlet.cmd.upgrade}
  fi

  ${rpm.scriptlet.echo.started}

  if [ $1 -ge 1 ]
  then
    :
  else
    if [ ! -f "$FILE_ZK_CONFIG_SH" ]
    then
      timestamp=$(date +"$LOG_DATE_FORMAT")

      echo
      echo "$timestamp [$cmd] File <$FILE_ZK_CONFIG_SH> does not exist; continuing."
    else
      source $FILE_ZK_CONFIG_SH
    fi

    #
    # Delete tunnels user.
    #
    userdel --force --remove $USER_TUNNEL

    #
    # Remove tunnels-related directories & files.
    #
    echo
    rm -rfv $DIR_TUNNELS_LOG

    #
    # Delete ZK user & group.  Do NOT remove /home/zookeeper home directory
    # because it may contain ZK keystore & truststore credentials that may be
    # reused on subsequent ZK installation.
    #
    userdel --force $USER_ZOOKEEPER
    groupdel $GROUP_ZOOKEEPER > /dev/null 2>&1

    #
    # Remove ZK-related directories & files.
    #
    echo
    for i in $DIR_ZK_ROOT $DIR_ZK_LOG $ZK_DATA_DIR
    do
      rm -rfv $i
    done

    echo
    rm -fv $LINK_ZOOKEEPER
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
