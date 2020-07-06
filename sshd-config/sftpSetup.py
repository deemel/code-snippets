#!/usr/bin/python

import filecmp
import getpass
import os
import pprint
import re
import shutil
import subprocess
import sys
import time

RC_SUCCESS = 0
RC_FAILURE = 1
MIN_ARGS = 0
MAX_ARGS = 1
PROFILE = os.getenv("HOME") + "/.bash_profile"

PARAM_HELP = "-h"
PARAM_UNINSTALL = "uninstall"

USER = "root"
FILE_GROUP = "/etc/group"
FILE_PASSWD = "/etc/passwd"
FILE_SSHD_CONFIG = "/etc/ssh/sshd_config"
SFTP_GROUP = "sftpusers"
SFTP_USER = "sftpuser"
SFTP_USER_HOME = "/var/log"
SFTP_USER_SHELL = "/sbin/nologin"
SFTP_SSHD_MATCH_GROUP = "Match Group " + SFTP_GROUP

command = os.path.basename(sys.argv[0])
pid = os.getpid()

def source(script, update = True):
  pipe = subprocess.Popen(". %s; env" % script, stdout = subprocess.PIPE, shell = True)
  data = pipe.communicate()[0]
  env = dict((line.split("=", 1) for line in data.splitlines()))

  if update:
    os.environ.update(env)

  return env

def grep(stringSearch, filename):
  fileObject = open(filename, "r")

  for line in fileObject:
    if re.search(stringSearch, line):
      fileObject.close()
      return True

  fileObject.close()
  return False

def displayUsage():
  print
  print "USAGE: " + command + " [" + PARAM_UNINSTALL + "]"
  print

  sys.exit(RC_FAILURE)

def sshdRestart(sshdConfigFileNew):
  if not(filecmp.cmp(sshdConfigFileNew, FILE_SSHD_CONFIG)):
    shutil.move(sshdConfigFileNew, FILE_SSHD_CONFIG)

    print "Restarting sshd."
    subprocess.call("service sshd restart", shell = True)

def end():
  timestamp = time.strftime("%Y/%m/%d %H:%M:%S")

  print
  print "SFTP setup script completed on " + timestamp + "."
  print

  sys.exit(RC_SUCCESS)

def main():
  if os.path.isfile(PROFILE):
    source(PROFILE)

  numArgs = len(sys.argv) - 1
  if numArgs < MIN_ARGS:
    displayUsage()

  if numArgs > MAX_ARGS:
    displayUsage()

  if numArgs == MAX_ARGS:
    if sys.argv[1] == PARAM_HELP:
      displayUsage()

    if sys.argv[1] != PARAM_UNINSTALL:
      displayUsage()

  i = getpass.getuser()
  if i != USER:
    print "Must be user <" + USER + "> to run this script; exiting."
    sys.exit(RC_FAILURE)

  timestamp = time.strftime("%Y/%m/%d %H:%M:%S")

  print
  print "----------------------------------------------------------------------"
  print "MFDD SFTP setup script invoked on " + timestamp + "."
  print "----------------------------------------------------------------------"
  print

  sshdConfigFilename = FILE_SSHD_CONFIG.split("/")[3]
  sshdConfigFileBackup = "/opt/mfdd/tmp/" + sshdConfigFilename + "." + str(pid) + ".0"
  sshdConfigFileNew = "/opt/mfdd/tmp/" + sshdConfigFilename + "." + str(pid) + ".new"

  print "Backing up file <" + FILE_SSHD_CONFIG + "> to <" + sshdConfigFileBackup +">."
  shutil.copy(FILE_SSHD_CONFIG, sshdConfigFileBackup)

  try:
    if sys.argv[1] == PARAM_UNINSTALL:
      print "Uninstalling MFDD SFTP setup."

      print "Deleting user <" + SFTP_USER + ">."
      subprocess.call("userdel " + SFTP_USER + " 2> /dev/null", shell = True)

      try:
        os.remove("/var/spool/mail/" + SFTP_USER)
      except OSError:
        pass

      print "Deleting group <" + SFTP_GROUP + ">."
      subprocess.call("groupdel " + SFTP_GROUP + " 2> /dev/null", shell = True)

      print "Unconfiguring sshd if necessary."
      subprocess.call("sed -e \"s/#Subsystem\\tsftp\\t\\/usr\\/libexec\\/openssh\\/sftp-server/Subsystem\\tsftp\\t\\/usr\\/libexec\\/openssh\\/sftp-server/g\" -e \"/Subsystem\\tsftp\\tinternal-sftp/d\" -e \"/^" + SFTP_SSHD_MATCH_GROUP + "/,+4d\" " + FILE_SSHD_CONFIG + " > " + sshdConfigFileNew, shell = True)

      sshdRestart(sshdConfigFileNew)
      end()
  except IndexError:
    pass

  if not(grep("^" + SFTP_GROUP, FILE_GROUP)):
    print "Adding group <" + SFTP_GROUP + ">."
    subprocess.call("groupadd " + SFTP_GROUP, shell = True)

  if not(grep("^" + SFTP_USER, FILE_PASSWD)):
    print "Adding user <" + SFTP_USER + ">."
    print "*** Please manually set password for user <" + SFTP_USER + "> (i.e., \"passwd " + SFTP_USER + "\") before 1st use."
    subprocess.call ("useradd -g " + SFTP_GROUP + " -d " + SFTP_USER_HOME + " -s " + SFTP_USER_SHELL + " -M " + SFTP_USER + " 2> /dev/null", shell = True)

  print "Configuring sshd's SFTP subsystem if necessary."
  subprocess.call("sed \"s/^Subsystem\\tsftp\\t\\/usr\\/libexec\\/openssh\\/sftp-server/#\\0\\nSubsystem\\tsftp\\tinternal-sftp/g\" " + FILE_SSHD_CONFIG + " > " + sshdConfigFileNew, shell = True)

  if not(grep("^" + SFTP_SSHD_MATCH_GROUP, sshdConfigFileNew)):
    print "Configuring sshd's SFTP match group."
    with open(sshdConfigFileNew, "a") as fileObject:
      fileObject.write("\n")
      fileObject.write("Match Group " + SFTP_GROUP + "\n")
      fileObject.write("       ChrootDirectory " + SFTP_USER_HOME + "\n")
      fileObject.write("       X11Forwarding no\n")
      fileObject.write("       AllowTcpForwarding no\n")
      fileObject.write("       ForceCommand internal-sftp\n")
    fileObject.close()

  print "Turning on SELinux ssh_chroot_full_access."
  subprocess.call("setsebool -P ssh_chroot_full_access on", shell = True)

  sshdRestart(sshdConfigFileNew)
  end()

main()
