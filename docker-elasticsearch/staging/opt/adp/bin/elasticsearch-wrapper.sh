#!/bin/bash

source /opt/adp/bin/base-common.sh

ES_ENV_PREFIX="ES_ENV_"

#
# Mostly from https://github.com/elastic/elasticsearch-docker/blob/5.2/build/elasticsearch/bin/es-docker
#

# Run Elasticsearch and allow setting default settings via env vars
#
# e.g. Setting the env var cluster.name=testcluster
#
# will cause Elasticsearch to be invoked with -Ecluster.name=testcluster
#
# see https://www.elastic.co/guide/en/elasticsearch/reference/current/settings.html#_setting_default_settings

es_opts=''

while IFS='=' read -r envvar_key envvar_value
do
    # Workaround for environments where dot character '.' is not supported in environment variable names
    #
    # e.g. Setting the env var ES_ENV_01=cluster.name=testcluster
    #
    # will cause Elasticsearch to be invoked with -Ecluster.name=testcluster
    if [[ "$envvar_key" =~ ^${ES_ENV_PREFIX}.* ]]
    then
        envvar_key=$(echo $envvar_value | cut -d'=' -f1)
        envvar_value=$(echo $envvar_value | cut -d'=' -f2)
    fi

    # Elasticsearch env vars need to have at least two dot separated lowercase words, e.g. `cluster.name`
    if [[ "$envvar_key" =~ ^[a-z]+\.[a-z]+ ]]
    then
        if [[ ! -z $envvar_value ]]; then
          es_opt="-E${envvar_key}=${envvar_value}"
          es_opts+=" ${es_opt}"
        fi
    fi
done < <(env)

# The virtual file /proc/self/cgroup should list the current cgroup
# membership. For each hierarchy, you can follow the cgroup path from
# this file to the cgroup filesystem (usually /sys/fs/cgroup/) and
# introspect the statistics for the cgroup for the given
# hierarchy. Alas, Docker breaks this by mounting the container
# statistics at the root while leaving the cgroup paths as the actual
# paths. Therefore, Elasticsearch provides a mechanism to override
# reading the cgroup path from /proc/self/cgroup and instead uses the
# cgroup path defined the JVM system property
# es.cgroups.hierarchy.override. Therefore, we set this value here so
# that cgroup statistics are available for the container this process
# will run in.
export ES_JAVA_OPTS="-Des.cgroups.hierarchy.override=/ $ES_JAVA_OPTS"


# Make sure if the JVM memory limits are set by environment variables
# then remove the default values from the jvm.options file

[[ $ES_JAVA_OPTS =~ '-Xms' ]] && sed -i 's/^-Xms/# -Xms/' $ES_ROOT/config/jvm.options
[[ $ES_JAVA_OPTS =~ '-Xmx' ]] && sed -i 's/^-Xmx/# -Xmx/' $ES_ROOT/config/jvm.options

command="$ES_ROOT/bin/elasticsearch ${es_opts} $ES_CMD_LINE_FLAGS"
initialize "$command"

$command
rc=$?

finalize "$command" $rc