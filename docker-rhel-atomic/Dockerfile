ARG VERSION_RHEL7_ATOMIC
FROM registry.access.redhat.com/rhel7-atomic:${VERSION_RHEL7_ATOMIC}

ARG JAVA_VERSION

ENV JAVA_HOME=/opt/jre${JAVA_VERSION}
ENV PATH=/opt/adp/bin:${JAVA_HOME}/bin:$PATH \
    IS_TERMINATE_S6="true" \
    S6_BEHAVIOUR_IF_STAGE2_FAILS=2 \
    S6_LOGGING=0 \
    BOOL_TRUE="true" \
    BOOL_FALSE="false" \
    SD_SERVER_PORT="8500" \
    SERVICE_NAME_LOG="com-jupiter-interface-log" \
    SERVICE_NAME_ELASTICSEARCH="com-jupiter-interface-elasticsearch" \
    APP_NAME="default" \
    SD_HEALTH_CHECK_INTERVAL="10s"

COPY target/packages/ /
COPY staging/ /

USER root

RUN set -ex && \
    \
    microdnf --nodocs install net-tools && \
    microdnf clean all && \
    rm /etc/yum.repos.d/jupiter-yum-server.repo && \
    bash /opt/adp/bin/adp-set-perms.sh && \
    true