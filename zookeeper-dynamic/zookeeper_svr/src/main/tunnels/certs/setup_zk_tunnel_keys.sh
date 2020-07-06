#!/bin/sh

${dir.tunnels.certs}/deploy_ssh_server.sh tunnel || exit 1
${dir.tunnels.certs}/deploy_ssh_client.sh tunnel || exit 1

exit 0
