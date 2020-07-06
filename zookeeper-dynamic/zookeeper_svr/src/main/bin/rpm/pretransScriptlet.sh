${rpm.scriptlet.shebang}

${rpm.scriptlet.debug}

${rpm.scriptlet.cmd.runtime}
${rpm.scriptlet.cmd.pretrans}
${rpm.scriptlet.cmd.short}
${rpm.scriptlet.cmd.log}

{
  ${rpm.scriptlet.echo.started}
  ${rpm.scriptlet.echo.completed}

  rc=$?
  exit $rc
} 2>&1 | tee $log

#
# RPM scriptlets MUST exit with 0 (i.e., success) exit code.  More info at
# https://fedoraproject.org/wiki/Packaging:Scriptlets.
#
${rpm.scriptlet.exit}
