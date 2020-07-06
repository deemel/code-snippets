ARG VERSION_COMMON_BASE_RHEL
FROM proj-pigs/base-rhel:${VERSION_COMMON_BASE_RHEL}

ARG ES_ROOT

ENV ES_ROOT="$ES_ROOT" \
    PATH=$ES_ROOT/bin:$PATH \
    SERVICE_NAME="$SERVICE_NAME_ELASTICSEARCH" \
    ES_CMD_LINE_FLAGS="--silent"

COPY target/packages/ /
COPY staging/ /

USER root

RUN set -ex && \
    \
    groupadd --system --gid 1000 elasticsearch && \
    useradd --system --uid 1000 --gid elasticsearch elasticsearch && \
    usermod --append --groups root elasticsearch && \
    mkdir -p $ES_ROOT/plugins && \
    for path in \
        $ES_ROOT/data \
        $ES_ROOT/logs \
        $ES_ROOT/config \
        $ES_ROOT/config/scripts \
    ; do \
      mkdir -p "$path"; \
    done && \
    chown -R elasticsearch:elasticsearch $ES_ROOT && \
    bash /opt/adp/bin/adp-set-perms.sh && \
    \
    ulimit -n 65536 && \
    elasticsearch --version && \
    true

VOLUME $ES_ROOT/data

EXPOSE 9200 9300

ENTRYPOINT ["/init"]
