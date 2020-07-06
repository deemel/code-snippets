#!/bin/bash

source /opt/adp/bin/base-common.sh

command="$LS_ROOT/bin/logstash $LS_CMD_LINE_FLAGS"
initialize "$command"

$command
rc=$?

finalize "$command" $rc
