Secure client-server communication is supported by ZooKeeper via SSL (see https://cwiki.apache.org/confluence/display/ZOOKEEPER/ZooKeeper+SSL+User+Guide).  However, key store and trust store credentials (i.e., JKS locations, passwords) are provided as clear text Java system properties at runtime.  This enhancement aims to make more secure key store and trust store credentials by not exposing them at ZooKeeper server/client startups and by storing them in predefined, permission-controlled, obfuscated files.  Written in Java.