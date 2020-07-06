#!/bin/bash

fileScript=$(readlink -f $0)
dirPostDeploy=$(dirname $fileScript)
dirScripts=$(dirname $dirPostDeploy)
dirLib=$dirScripts/lib
vmHost=$(hostname -s)
oamGroup=$(/opt/mfdd/latest/base/bin/cli -c "cm-list" | grep OamNode=${vmHost}$ | cut -d',' -f4 | cut -d'=' -f2)

$dirLib/zkrepl-run.sh $oamGroup

exit $?
