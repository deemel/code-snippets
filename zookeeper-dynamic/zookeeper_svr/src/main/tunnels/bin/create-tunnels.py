#!/usr/bin/python
import os,sys

SSH_TUNNEL='ssh-tunnel'
SSH_DUMMY='ssh-dummy'
TUNNEL_DELIM='.'
TARGETDIR='${dir.tunnels}'
TARGETDIRBIN=TARGETDIR+'/bin'
TARGETDIRCONF=TARGETDIR+'/conf'

if not os.path.exists(TARGETDIRBIN):
   os.makedirs(TARGETDIRBIN)
if not os.path.exists(TARGETDIRCONF):
   os.makedirs(TARGETDIRCONF)

os.chdir(TARGETDIRBIN)

def parse_vars(var):
   map={'local':{},'remote':{}}
   varr=var.split(',')
   for hostport in varr:
      harr=hostport.split(':')
      if(len(harr) == 3):
         host=harr[0].split('.')[0]
         map['local'][host] = harr[1]
         map['remote'][host] = harr[2]

   return map

def create_tunnel_link(host,svc_type,ports_map,this_host):
    #print ports_map
    port_loc = ports_map['local'][host]
    port_rem = ports_map['remote'][host]
    tun_type = SSH_DUMMY if this_host == host else SSH_TUNNEL
    tun_str = SSH_TUNNEL + TUNNEL_DELIM + port_loc + TUNNEL_DELIM + host + TUNNEL_DELIM + port_loc
    print "creating tunnel " + tun_str
    if os.path.exists(tun_str):
       os.remove(tun_str)
    os.symlink(tun_type,tun_str)
    return ("{0}-{1}-{2}".format(svc_type,port_loc,host), TARGETDIRBIN + "/" + tun_str)

PROF_START="""\
{
   "systemName" : "mfdd",
    "profiles" : [ {
        "name" : "ZKSSH-SERVER",
        "services" : [ 
"""
PROFILE="            { \"name\" : \"%s\" }"
PROF_END="""
        ]
    } ]
}
"""

SRVC_START="""\
{
    "systemName" : "mfdd",
    "services" : [ {
"""
SERVICE="""\
        "name" : "{0}",
        "resourceID" : "1.1.1.11.1.1",
        "expectedOperationalState" : "UP",
        "isLinuxActionScript" : "false",
        "initScript" : "{1}",
        "autoRestart" : "true",
        "autoRestartAllowedRetryAttempts" : "666"\
"""
SRVC_END="""
    } ]
}
"""

def writeFiles(servers):
    prof = open("{0}/zkssh-profiles.json".format(TARGETDIRCONF), "w")
    prof.write(PROF_START)
    serverNames = [PROFILE % (service) for (service, tunnel) in servers]
    prof.write(",\n".join(serverNames))
    prof.write(PROF_END)
    prof.close()

    srvc = open("{0}/zkssh-services.json".format(TARGETDIRCONF), "w")
    srvc.write(SRVC_START)
    serverSrvcs = [SERVICE.format(service, tunnel) for (service, tunnel) in servers]
    srvc.write("\n    },{\n".join(serverSrvcs))
    srvc.write(SRVC_END)
    srvc.close()

#
#main()
#

this_svr=os.getenv("ZK_THIS_SERVER")

peer_env=os.getenv("ZK_SSH_TUNNEL_PEERS")
if (peer_env == None):
   print "Error: Environment variable ZK_SSH_TUNNEL_PEERS is not defined"
   sys.exit(1)

peer_ports=parse_vars(peer_env)
hosts = [host for host in peer_ports['local']]
peers = [create_tunnel_link(host,"zks",peer_ports,this_svr)   for host in hosts]

if(len(peer_ports['local']) == 0):
   print "Error: Environment variable ZK_SSH_TUNNEL_PEERS value '%s' has invalid format" %(peer_env)
   sys.exit(1)

leader_env=os.getenv("ZK_SSH_TUNNEL_LEADERS")
if (leader_env == None):
   print "Error: Environment variable ZK_SSH_TUNNEL_LEADERS is not defined"
   sys.exit(1)

leader_ports=parse_vars(leader_env)
leaders = [create_tunnel_link(host,"zks",leader_ports,this_svr) for host in hosts]

if(len(leader_ports['local']) == 0):
   print "Error: Environment variable ZK_SSH_TUNNEL_LEADERS value '%s' has invalid format" %(leader_env)
   sys.exit(1)

print "hosts  :", hosts
print "leaders:", leaders
print "peers  :", peers

if (peer_env != None):
   writeFiles(peers + leaders)
