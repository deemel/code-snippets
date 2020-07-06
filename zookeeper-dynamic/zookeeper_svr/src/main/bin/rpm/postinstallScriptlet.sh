${rpm.scriptlet.shebang}

${rpm.scriptlet.debug}
${cmd.source.zk.mfdd.lib}

${rpm.scriptlet.cmd.runtime}
${rpm.scriptlet.cmd.postinstall}
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

IS_RPM_RUNNING="$STR_FALSE"
if [ "$cmdRuntime" != "$cmd" ]
then
  #
  # RPM package manager is running this script.
  #
  IS_RPM_RUNNING="$STR_TRUE"
fi

{
  ${rpm.scriptlet.check.num.args}

  if [ $1 -ge 2 ]
  then
    ${rpm.scriptlet.cmd.upgrade}
  fi

  if [ "$IS_RPM_RUNNING" = "$STR_TRUE" ]
  then
    if [ -f "$FILE_ZK_CONFIG_DAT" ]
    then
      source $FILE_ZK_CONFIG_DAT

      if [ -z "$ZK_RPM_NO_SCRIPTS" ]
      then
        ZK_RPM_NO_SCRIPTS="$ZK_RPM_NO_SCRIPTS_DEF"
      fi

      if [ "$ZK_RPM_NO_SCRIPTS" = "$STR_TRUE" ]
      then
        ${rpm.scriptlet.echo.started}

        echo2 "[$cmd] ZK_RPM_NO_SCRIPTS <$ZK_RPM_NO_SCRIPTS> specified in file <$FILE_ZK_CONFIG_DAT> or defaulted to <$ZK_RPM_NO_SCRIPTS_DEF>; exiting <$RC_SUCCESS>."
        echo

        ${rpm.scriptlet.echo.completed}
        exit $RC_SUCCESS
      fi
    fi
  fi

  ${rpm.scriptlet.echo.started}

  if [ $1 -ge 2 ]
  then
    #
    # Move RPM-saved files back to original locations, updating them to
    # incorporate newly introduced changes if necessary.  Expect this piece of
    # code to possibly change when future ZK versions are introduced.
    #
    for i in "$FILE_ZOO_CFG" "$FILE_ZOOKEEPER_ENV"
    do
      fileRpmSave="$i.$EXT_RPMSAVE"
      if [ ! -f "$fileRpmSave" ]
      then
        continue
      fi

      mv -fv $fileRpmSave $i
      chown $USER_ZOOKEEPER:$GROUP_ZOOKEEPER $i
    done

    #
    # Get init process.  Start ZK service.
    #
    initProcess=""
    funcToCall="initProcessGet initProcess"
    call "$funcToCall"

    case "$initProcess" in
      "$INIT_PROCESS_SYSVINIT" | "$INIT_PROCESS_UPSTART")
        call "zkServiceSetUpSysVinit $STR_TRUE"
        ;;

      "$INIT_PROCESS_SYSTEMD")
        call "zkServiceSetUpSystemd $STR_TRUE"
        ;;

      *)
        rc=$RC_FAILURE
        if [ "$IS_RPM_RUNNING" = "$STR_TRUE" ]
        then
          rc=$RC_SUCCESS
        fi

        echo2 "[$cmd] Unsupported init process <$initProcess>; exiting <$rc>."
        exit $rc
    esac

    call "zkCrontabAdd"
  else
    call "mfdd3ppLinkAdd"
    call "tunnelsInitScriptInstall"
    call "tunnelsEnable"
    call "zkConfigDatRead"
    call "zkEnvVarsAdd"
    call "zooCfgCreate"
    call "zooCfgDynamicCreate"
    call "zkMyidInit"

    #
    # This script is called at this point solely for purpose of creating,
    # starting SSH tunnels (if necessary) BEFORE starting up ZK.
    #
    call "$CMD_ZOO_CFG_DYNAMIC_PARSE"

    #
    # Get init process.  Set up ZK service.
    #
    initProcess=""
    funcToCall="initProcessGet initProcess"
    call "$funcToCall"

    case "$initProcess" in
      "$INIT_PROCESS_SYSVINIT" | "$INIT_PROCESS_UPSTART")
        call "zkServiceSetUpSysVinit"
        ;;

      "$INIT_PROCESS_SYSTEMD")
        call "zkServiceSetUpSystemd"
        ;;

      *)
        rc=$RC_FAILURE
        if [ "$IS_RPM_RUNNING" = "$STR_TRUE" ]
        then
          rc=$RC_SUCCESS
        fi

        echo2 "[$cmd] Unsupported init process <$initProcess>; exiting <$rc>."
        exit $rc
    esac

    call "zkPendingSyncsWait"
    call "zkReconfigAdd"
    call "zkPendingSyncsWait"
    call "zkCrontabAdd"
  fi

  ${rpm.scriptlet.echo.completed}

  rc=$?
  exit $rc
} 2>&1 | tee $log

#
# Exit return code is return code from 1st command in pipe, i.e., stdout &
# stderr redirection.  More info at
# https://unix.stackexchange.com/questions/186826/parent-script-continues-when-child-exits-with-non-zero-exit-code.
#
rc=${PIPESTATUS[0]}

if [ "$IS_RPM_RUNNING" = "$STR_TRUE" ]
then
  #
  # RPM scriptlets MUST exit with 0 (i.e., success) exit code.  More info at
  # https://fedoraproject.org/wiki/Packaging:Scriptlets.
  #
  rc=$RC_SUCCESS
fi

exit $rc
