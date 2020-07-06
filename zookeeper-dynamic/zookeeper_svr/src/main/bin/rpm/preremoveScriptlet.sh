${rpm.scriptlet.shebang}

${rpm.scriptlet.debug}
${cmd.source.zk.mfdd.lib}

${rpm.scriptlet.cmd.runtime}
${rpm.scriptlet.cmd.preremove}
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
    zkCrontabRemove
    zkReconfigRemove

    #
    # Get init process.  Tear down ZK service.
    #
    initProcess=""
    initProcessGet initProcess

    case "$initProcess" in
      "$INIT_PROCESS_SYSVINIT" | "$INIT_PROCESS_UPSTART")
        zkServiceTearDownSysVinit
        ;;

      "$INIT_PROCESS_SYSTEMD")
        zkServiceTearDownSystemd
        ;;

      *)
        echo2 "[$cmd] Unsupported init process <$initProcess>; continuing."
    esac

    zooCfgRemove
    tunnelsStop
    tunnelsStatus
    tunnelsDisable
    tunnelsInitScriptUninstall
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
