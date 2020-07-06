#!/bin/bash

RC_SUCCESS=0
RC_FAILURE=1

ES_IMAGE="proj-pigs/jupiter-data-search-engine"
FB_IMAGE="proj-pigs/jupiter-log-shipper"
ES_PORT=9200
CURL_TRY_MAX=10

DIR_ROOT_TEST="/root/adp/fb-to-es"
FILENAME_FB_CONF="filebeat.yml"
FILE_TO_HARVEST="$DIR_ROOT_TEST/SecurityAuth.audit"
FB_CONF_LOCAL="$DIR_ROOT_TEST/$FILENAME_FB_CONF"
FB_CONF_CONTAINER="/opt/adp/conf/$FILENAME_FB_CONF"
MSG_PREFIX="hello world"

function clean() {
  docker rm -f $(docker ps -a -q) 
  docker volume prune -f
}

function query() {
  local arg="$1"

  curl -X GET "$esHost:$ES_PORT$arg" -H'Content-Type: application/json' -d'{"size" : 100}'
  return $?
}

function getNumMessages() {
  local output=$(query "/_search?pretty" 2> /dev/null)
  local rc=$?

  local numMessages=0
  if [ $rc -eq $RC_SUCCESS ]
  then
    output=$(echo -e "$output" | grep "$MSG_PREFIX")
    rc=$?

    if [ $rc -eq $RC_SUCCESS ]
    then
      output=$(echo -e "$output" | sort -n -k5)
      numMessages=$(echo -e "$output" | wc -l)
    fi
  fi

  if [ ! -z "$output" ]
  then
    echo -e "$output"
  fi

  echo "# messages in Elasticsearch: $numMessages"

  return $numMessages
}

clean > /dev/null 2>&1

esContainerId=$(docker run -dti $ES_IMAGE)
esContainerId=$(echo $esContainerId | cut -c 1-12)
esHost=$(docker exec -ti $esContainerId cat /etc/hosts | grep $esContainerId | awk '{print $1}')
fbContainerId=$(docker run -dti -e ELASTICSEARCH_HOSTS="$esHost:$ES_PORT" -e FB_CONF="$FB_CONF_LOCAL" -v $FB_CONF_LOCAL:$FB_CONF_CONTAINER -v $DIR_ROOT_TEST:$DIR_ROOT_TEST $FB_IMAGE)
fbContainerId=$(echo $fbContainerId | cut -c 1-12)

echo
echo "Elasticsearch container ID: $esContainerId"
echo "Elasticsearch host: $esHost"
echo "Filebeat container ID: $fbContainerId"

i=1
while [ $i -le $CURL_TRY_MAX ]
do
  jitter=$(($RANDOM % 30 + 1))
  sleep $jitter

  query > /dev/null 2>&1

  rc=$?
  if [ $rc -ne $RC_SUCCESS ]
  then
    i=$(($i + 1))
    continue
  fi

  echo
  echo "Elasticsearch is running."

  break
done

if [ $i -gt $CURL_TRY_MAX ]
then
  echo "Elasticsearch is not running; exiting <$rc>."
  exit $rc
fi

getNumMessages

> $FILE_TO_HARVEST
numMessages=$(($RANDOM % 30 + 1))

echo "Adding <$numMessages> messages to file <$FILE_TO_HARVEST>."

i=1
while [ $i -le $numMessages ]
do
  message="$MSG_PREFIX $i"

  echo
  echo "Writing <$message> to file <$FILE_TO_HARVEST>."
  echo "$message" >> $FILE_TO_HARVEST
  i=$(($i + 1))

  sleep 10
  getNumMessages
done

echo
sleep 10
getNumMessages

echo

i=$?
if [ $i != $numMessages ]
then
  echo "Sent messages <$numMessages> != received messages <$i>."
  exit $RC_FAILURE
fi

echo "Sent messages <$numMessages> = received messages <$i>."
exit $RC_SUCCESS
