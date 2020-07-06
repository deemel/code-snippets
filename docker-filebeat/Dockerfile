ARG VERSION_RHEL7_ATOMIC
FROM registry.access.redhat.com/rhel7-atomic:${VERSION_RHEL7_ATOMIC}

ENV BOOL_TRUE="true" \
    BOOL_FALSE="false" \
    DIR_OPT="/opt"
ENV ADP_ROOT="$DIR_OPT/adp"
ENV ADP_BIN="$ADP_ROOT/bin" \
    ADP_CONF="$ADP_ROOT/conf" \
    FB_ROOT="$DIR_OPT/filebeat"
ENV PATH=$FB_ROOT:$ADP_BIN:$PATH \
    FB_CONF="$ADP_CONF/filebeat.conf" \
    FB_YML="$ADP_CONF/filebeat.yml" \
    FILEBEAT_PROSPECTORS_LOG_FIELDS_LOGPLANE="applogs" \
    FILEBEAT_OUTPUT_CONSOLE_IS_ENABLED="$BOOL_FALSE" \
    FB_CMD_LINE_FLAGS="-e -path.home $FB_ROOT"

COPY staging/ /
COPY target/lib/filebeat-wrapper.bin $ADP_BIN/
COPY target/lib/filebeat $FB_ROOT/

CMD ["/opt/adp/bin/filebeat-wrapper.bin"]
