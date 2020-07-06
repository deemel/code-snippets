#!/usr/bin/python -Btt

import os
import subprocess
import sys
import json

here = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(os.path.dirname(here), 'lib'))

from Ini import IniConfigParser

from VmManagerScript import VmManagerScript
from SshAgent import SshAgent


class ConfigurePlatformServices(VmManagerScript):
    def main(self):
        if self.level == self.GROUP_LEVEL:
            return 0

        vmName = sys.argv[2]
        dv = self.config.getDv(vmName)

        dvDir = os.path.abspath(os.path.join(here, "..", ".."))
        ansibleDir = os.path.join(dvDir, "ansible", "platform")
        inventory = os.path.join(ansibleDir, "inventory")

        playbooks = []
        playbooks.append(os.path.join(ansibleDir, "lrubef-config-1-main.yaml"))

        # Read package.json
        infoFilePath = os.path.join(dvDir, "package.json")
        if os.path.exists(infoFilePath):
            try:
                with open(infoFilePath) as fp:
                    infoData = json.load(fp)
                    dvRepoUrl = infoData.get("packages",
                                             {}).get("repository",
                                                     {}).get("baseurl")
                    dvRepoUrl_ssl = infoData.get("packages",
                                                 {}).get("repository",
                                                         {}).get("baseurl_ssl")
            except Exception as e:
                print("Failed to read %s: %s" % (infoFilePath, e))
                return 1

        variables = {}
        variables["DV_NAME"] = dv
        if dvRepoUrl_ssl:
            variables["DV_REPO_URL_SSL"] = dvRepoUrl_ssl
        elif dvRepoUrl:
            variables["DV_REPO_URL"] = dvRepoUrl

        os.environ["ANSIBLE_HOST_KEY_CHECKING"] = "False"
        os.environ["VMMANAGER_INI"] = sys.argv[1]
        variables["INI"] = os.environ.get("VMMANAGER_INI")

        cmd = ["ansible-playbook"]
        ini = IniConfigParser(variables["INI"])
        variables["OAM_GROUP_NAME"] = ini.getGroupForVm(sys.argv[2])

        for key, value in variables.iteritems():
            cmd += ["-e", "%s=%s" % (key, value)]
        cmd += ["-i", inventory]
        cmd += playbooks
        cmd += ["--limit", vmName]

        with SshAgent(auto_close=False) as agent:
            agent.add_key("/repository/ansible/.ssh/id_rsa_ansible")
            print(cmd)
            subprocess.check_call(cmd, cwd=ansibleDir)

        return 0


if __name__ == "__main__":
    try:
        sys.exit(ConfigurePlatformServices(sys.argv).main())
    except Exception, e:
        print(e)
        sys.exit(1)
