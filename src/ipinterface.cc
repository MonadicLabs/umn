
#include <sys/types.h>
#include <ifaddrs.h>

#include <iostream>

#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ipinterface.h"

using namespace std;

void novadem::ip::IpInterface::run()
{
    // Start emitting beacon
    startEmittingBeacon();

    // Start receiving beacon
    startReceivingBeacon();

    // Boucle d'ecoute
    while(!hasToStop())
    {
        sleep(1);
    }
    //

    //    _beaconEmissionThread.join();
    //    _beaconReceptionThread.join();

}

void novadem::ip::IpInterface::emitBeacon()
{
    int fd;
    struct sockaddr_in addr;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        // exit(1);
    }

    /* set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("239.0.0.1");
    addr.sin_port=htons(59620);

    ShortId nodeId = _parentNode->id();
    const char * message = nodeId.toString().c_str();

    while (!hasToStop())
    {
        if (sendto(fd, message, strlen(message), 0,(struct sockaddr *) &addr, sizeof(addr)) < 0)
        {

        }
        sleep(1);

        // Decrease TTL of all our references
        for (auto it = _nodeRefs.cbegin(); it != _nodeRefs.cend() /* not hoisted */; /* no increment */)
        {
            it->second->_ttl--;
            if ( it->second->_ttl <= 0 )
            {
                // cout << "Removing node..." << endl;
                _parentNode->removeNeighbourReference( ShortId( it->second->_nodeId ), this );
                _nodeRefs.erase(it++);    // or "it = m.erase(it)" since C++11
            }
            else
            {
                // cout << "NODEID: " << it->second->_nodeId << " NODEIP:" << it->first << " TTL:" << it->second->_ttl << endl;
                ++it;
            }
        }

    }
}

void novadem::ip::IpInterface::receiveBeacon()
{
    ShortId parentId = _parentNode->id();

    struct ip_mreq mreq;
    int fd;
    struct sockaddr_in addr;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        // exit(1);
    }

    /**** MODIFICATION TO ORIGINAL */
    uint yes = 1;
    /* allow multiple sockets to use the same PORT number */
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
        perror("Reusing ADDR failed");
        exit(1);
    }
    /*** END OF MODIFICATION TO ORIGINAL */

    /* set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port=htons(59620);

    /* bind to receive address */
    if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    /* use setsockopt() to request that the kernel join a multicast group */
    mreq.imr_multiaddr.s_addr=inet_addr("239.0.0.1");
    mreq.imr_interface.s_addr=inet_addr(_ownIpAddr.c_str());
    if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    char msgbuf[1024];
    /* now just enter a read-print loop */
    while (!hasToStop()) {
        int nbytes;
        uint addrlen=sizeof(addr);
        if ((nbytes=recvfrom(fd,msgbuf,1024,0,
                             (struct sockaddr *) &addr,&addrlen)) < 0)
        {
            perror("recvfrom");
            exit(1);
        }
        else
        {
            ShortId receivedId;
            // Check if is a valid ShortId
            try{
                receivedId = ShortId( std::string(msgbuf) );
            }
            catch( std::exception ){
                continue;
            }

            // Check if it's not our own beacon
            if( receivedId != parentId )
            {
                std::string nodeIp = inet_ntoa(addr.sin_addr);
                // cout << "nodeIp=" << nodeIp << endl;

                // Keep track of received IPs
                if( _nodeRefs.find(nodeIp) == _nodeRefs.end() )
                {
                    // cout << "new ref" << endl;
                    IpNodeReference * ipref = new IpNodeReference();
                    ipref->_ttl = 10;
                    ipref->_nodeId = receivedId.toString();
                    _nodeRefs.insert( make_pair(nodeIp, ipref) );
                }
                else
                {
                    // cout << "existing ref" << endl;
                    _nodeRefs[ nodeIp ]->_ttl = 10;
                }
                _parentNode->addNeighbourReference( ShortId(_nodeRefs[ nodeIp ]->_nodeId), this );
            }
            else
            {
                // cout << "received our own id" << endl;
            }
        }
    }

}

bool novadem::ip::IpInterface::checkNetworkInterfaceExists( const std::string& ifName )
{
    bool ret = false;
    struct ifaddrs *addrs,*tmp;
    getifaddrs(&addrs);

    struct sockaddr_in *sa;
    char *addr;
    for (tmp = addrs; tmp; tmp = tmp->ifa_next) {
        if (tmp->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) tmp->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            // printf("Interface: %s\tAddress: %s\n", tmp->ifa_name, addr);
            if( tmp->ifa_name == ifName )
            {
                _ownIpAddr = addr;
                ret = true;
                break;
            }
        }
    }

    freeifaddrs(addrs);
    return ret;

}
