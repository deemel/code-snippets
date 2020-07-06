#!/bin/bash

# Script to assist in building smaller size Docker images.  Achieved
# primarily by reducing use of ADD or COPY instructions in Dockerfile for
# copying large files into Docker image, thereby eliminating Docker image
# layers that are most common causes for large Docker image size.  Before
# building Docker image, HTTP server is started on build machine to serve
# files rooted in current working directory (e.g., target).  A file
# containing build machine's URL is copied (perhaps via COPY instruction)
# to Docker image for subsequent use during Docker image build.  If a large
# file needs to be copied into Docker image, ensure it can be served by
# HTTP server.  In Dockerfile, use curl or wget (in chained RUN
# instruction) with build machine's URL (obtained from saved build URL
# file) and path to file in order to retrieve file into image.  If
# necessary, remove retrieved file after using it (e.g., after
# installation).

#set -x

RC_SUCCESS=0
RC_FAILURE=1
MIN_ARGS=1
MAX_ARGS=3
TIMESTAMP_FORMAT="%Y/%m/%d %H:%M:%S"

OP_START="start"
OP_STOP="stop"
DEF_PORT_HTTP_SERVER=7170
DEF_FILE_BUILD_URL="./staging/tmp/build_url.txt"

command="$(basename $0)"

function displayUsage() {
  local rc=$1

  echo
  echo "USAGE: $command {start|stop} [HTTP server port] [build URL file]"
  echo "  Default HTTP server port: $DEF_PORT_HTTP_SERVER"
  echo "  Default build URL file: $DEF_FILE_BUILD_URL"
  echo
  echo "  Current working directory: $cwd"
  echo

  exit $rc
}

function initialize() {
  local command="$1"
  local timestamp=$(date +"$TIMESTAMP_FORMAT")

  echo
  echo "----------------------------------------------------------------------"
  echo "\"$command\" invoked on $timestamp."
  echo "----------------------------------------------------------------------"
  echo

  return $?
}

function finalize() {
  local command="$1"
  local rc=$2
  local timestamp=$(date +"$TIMESTAMP_FORMAT")

  echo
  echo "\"$command\" completed with status <$rc> on $timestamp."
  echo

  exit $rc
}

function cleanup() {
  # Kill processes using HTTP server port.
  fuser -k $portHttpServer/tcp
}

######################################################################
# main
######################################################################

cwd="$(pwd)"

if [ $# -lt $MIN_ARGS ]
then
  displayUsage
fi

if [ $# -gt $MAX_ARGS ]
then
  displayUsage
fi

op="$1"
portHttpServer=$2
fileBuildUrl="$3"

case "$op" in
  "$OP_START")
    ;;

  "$OP_STOP")
    ;;

  *)
    displayUsage
    ;;
esac

if [ -z "$portHttpServer" ]
then
  portHttpServer=$DEF_PORT_HTTP_SERVER
fi

if [ -z "$fileBuildUrl" ]
then
  fileBuildUrl="$DEF_FILE_BUILD_URL"
fi

command="$command $op $portHttpServer $fileBuildUrl"
initialize "$command"

case "$op" in
  "$OP_START")
    # Save build machine's URL.
    mkdir -p $(dirname $fileBuildUrl)
    echo http://$(hostname -f):$portHttpServer > $fileBuildUrl

    # Start HTTP server.
    cleanup
    python -m SimpleHTTPServer $portHttpServer
    ;;

  "$OP_STOP")
    cleanup
    ;;
esac

rc=$RC_SUCCESS
finalize "$command" $rc
