#!/usr/bin/python -Btt

import os
import subprocess
import sys
import json

here = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(os.path.dirname(here), 'lib'))

from VmManagerScript import VmManagerScript
from SshAgent import SshAgent


class RunRteConfig(VmManagerScript):
    def main(self):
        if self.level != self.VM_LEVEL:
            return 0

        vmName = sys.argv[2]
        dv = self.config.getDv(vmName)
        vmGroup = self.config.getGroupForVm(vmName)

        basename = os.path.basename(here)
        dvDir = os.path.abspath(os.path.join(here, "..", ".."))
        ansibleDir = os.path.join(dvDir, "ansible", "platform")
        inventory = os.path.join(ansibleDir, "inventory")

        playbooks = []
        playbooks.append(os.path.join(ansibleDir, "lrurte-config-2-main.yaml"))

        dvRepoUrl = os.path.join(dvDir, "packages")

        variables = {}
        variables["DV_NAME"] = dv
        variables["DV_REPO_DIR"] = dvRepoUrl
        variables["LRURTE_INIFILE"] = sys.argv[1]
        variables["OAM_GROUP"] = vmGroup
        variables["OAM_NODE"] = vmName

        if (basename == 'postdeploy'):
            variables["LRURTE_POSTDEPLOY"] = "True"
        else:
            variables["LRURTE_POSTDEPLOY"] = "False"

        os.environ["ANSIBLE_HOST_KEY_CHECKING"] = "False"
        os.environ["VMMANAGER_INI"] = sys.argv[1]

        cmd = ["ansible-playbook"]

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
        sys.exit(RunRteConfig(sys.argv).main())
    except Exception, e:
        print(e)
        sys.exit(1)
