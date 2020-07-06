ARG VERSION_COMMON_BASE_RHEL
FROM proj-pigs/base-rhel:${VERSION_COMMON_BASE_RHEL}

ARG LS_ROOT

ENV LS_ROOT="$LS_ROOT"
ENV PATH=$LS_ROOT/bin:$PATH \
    SERVICE_NAME="$SERVICE_NAME_LOG" \
    SD_DISCOVER_INTERVAL_ELASTICSEARCH=60 \
    FILE_LOGSTASH_CONF="$LS_ROOT/config/logstash.conf" \
    LOGSTASH_OUTPUT_STDOUT_IS_ENABLED="$BOOL_FALSE" \
    SD_ELASTICSEARCH_ENABLED="$BOOL_TRUE" \
    FILE_OUTPUT_RAW_ENABLED="$BOOL_FALSE" \
    FILE_OUTPUT_UID=1000 \
    FILE_OUTPUT_GID=1000 \
    LOGSTASH_INPUT_BEATS_PORT=5045 \
    LOGSTASH_INPUT_TCP_PORT_SYSLOG=5014 \
    LOGSTASH_INPUT_TCP_PORT_REST=7170 \
    LOGSTASH_INPUT_UDP_PORT=5014 \
    LS_LOG_LEVEL="error"
ENV LS_CMD_LINE_FLAGS="--log.level $LS_LOG_LEVEL --config.reload.automatic --path.config $FILE_LOGSTASH_CONF"

COPY target/classes/packages/ /
COPY staging/ /

USER root

RUN set -ex && \
    \
    groupadd --system --gid 1000 logstash && \
    useradd  --system --uid 1000 --gid logstash logstash && \
    usermod --append --groups root logstash && \
    \
    cp -p /opt/adp/conf/logstash.conf.template $FILE_LOGSTASH_CONF && \
    \
    chown -R logstash:logstash $LS_ROOT && \
    bash /opt/adp/bin/adp-set-perms.sh && \
    logstash --version && \
    logstash-plugin list --verbose && \
    \
    true

EXPOSE 5014 5014/udp 5045 5080 9600

ENTRYPOINT ["/init"]
