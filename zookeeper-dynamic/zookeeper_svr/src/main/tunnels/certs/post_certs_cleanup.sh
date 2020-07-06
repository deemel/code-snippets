#!/bin/bash

# Modify user permissions for X.509 users

CERTDIR=/etc/opt/mfdd

if [ ! -d $CERTDIR ]
then
  echo "Error: $CERTDIR not found"
  exit 1
fi

cd $CERTDIR

for usrdir in `ls -d .??*`
do
   username=`echo $usrdir | sed -s 's/^\.//g'`

   echo fixing permissions for user $username

   grp=`groups $username | cut -d" " -f3`

   if [ -z ${grp} ]
   then
     echo "Error user:$username does not exist"
     exit 1
   fi

   chown -R  "$username":"$grp" $usrdir
   chmod 700 $usrdir

   if [ $? == 1 ]
   then
      echo "Error while changing permissions for user: $usrname"
      exit 1
   fi
done

echo removing intermediate files
rm -rf /opt/mfdd/tmp/SSH_DST
exit 0
