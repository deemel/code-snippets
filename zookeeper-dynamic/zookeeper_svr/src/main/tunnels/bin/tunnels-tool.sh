#!/bin/bash

#
# tunnels       This shell script takes care of starting and stopping
#               the SSH tunnels for accessing the zookeeper ensemble.
#
# chkconfig: 2345 60 20
# description: Controls SSH tunnels to access the ZooKeeper ensmble.

### BEGIN INIT INFO
# Provides:         tunnels
# Required-Start:   $all
# Required-Stop:    $all
# Default-Start:    2 3 4 5
# Default-Stop:     0 1 6
# Short-Description: zookeeper SSH tunnel daemon.
# Description: Controls SSH tunnels to access the ZooKeeper ensmble.
### END INIT INFO


if [ $# != 1 ]
then
    echo "Usage: `basename $0` [start|stop|status]"
    exit 1
fi

cmd=$1
cd ${dir.tunnels.bin}
log="${dir.tunnels.log}/tunnels-tool.log"

retcode=0
elapsed=0
wait=1
giveup=60

while [ $elapsed -le $giveup ]
do
    for tun in ssh-tunnel.*.*.*
    do
        if [ -L "$tun" ]
        then
            ./$tun $cmd
            retcode=$(($retcode||$?))
        fi
    done

    case $cmd in
        start)
            now=$(date)
            if [ $retcode -eq 0 ] ; then
                echo "${now}: started, retcode is $retcode, elapsed is $elapsed" >> $log
                break
            else
                echo "${now}: starting, retcode is $retcode, elapsed is $elapsed" >> $log
                sleep $wait
            fi
            ;;
        stop)
            now=$(date)
            echo "${now}: stopped, retcode is $retcode" >> $log
            break
            ;;
        *) break;;
    esac

    elapsed=$((elapsed+${wait}))
done

exit $retcode
