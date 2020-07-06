    {"siteId":"{{zk_siteId}}","hosts":[{{#zk_hosts}}"{{.}}",{{/zk_hosts}}],"port":{{zk_port}}{{^zk_port}}{{ZK_PORT_DEF}}{{/zk_port}}},
