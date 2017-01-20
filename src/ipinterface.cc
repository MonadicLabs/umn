
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

#include <uv.h>

using namespace std;

unsigned int hashstr(const char *str)
{
    unsigned int h;
    unsigned char *p;

    h = 0;
    for (p = (unsigned char*)str; *p != '\0'; p++)
        h = 37 * h + *p;
    int startRange = 1500;
    int ret = (h + startRange) % (65536-startRange); // or, h % ARRAY_SIZE;
    return ret;
}

void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf) {
    buf->base = (char*)malloc(size);
    buf->len = size;
}

void on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {

    if( nread <= 0 )
        return;

    cout << "popo" << endl;
    cout << "Received beacon from ";
    sockaddr_in* client = (sockaddr_in*)addr;
    printf("%d.%d.%d.%d\n",
      int(client->sin_addr.s_addr&0xFF),
      int((client->sin_addr.s_addr&0xFF00)>>8),
      int((client->sin_addr.s_addr&0xFF0000)>>16),
      int((client->sin_addr.s_addr&0xFF000000)>>24));
}

void umn::ip::IpInterface::run()
{
    std::string  strAddr;
    if( findInterfaceAddress( "wlan0", strAddr ) )
    {
        _ownIpAddr = strAddr;
        cout << "own ip is: " << _ownIpAddr << endl;
    }

    startBeaconReception();

    // Start emitting beacon
    startEmittingBeacon();

    // Start receiving beacon
    startReceivingBeacon();

    // Boucle d'ecoute
    while(!hasToStop())
    {
        // Wait for messages to arrive
        uv_run(_uvLoop, UV_RUN_DEFAULT);
    }
    //

}

void umn::ip::IpInterface::emitBeacon()
{
    int fd;
    struct sockaddr_in addr;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        cout << "nope?" << endl;
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
        cout << "emit." << endl;
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

void umn::ip::IpInterface::receiveBeacon()
{
    ShortId parentId = _parentNode->id();

    struct ip_mreq mreq;
    int fd;
    struct sockaddr_in addr;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        exit(1);
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
        cout << "receive." << endl;
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
            cout << "nbytes=" << nbytes << endl;
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
                int recvPort = hashstr(  receivedId.toString().c_str() );
                cout << "nodeIp=" << nodeIp << " - " << recvPort << endl;

                // Keep track of received IPs
                if( _nodeRefs.find(nodeIp) == _nodeRefs.end() )
                {
                    cout << "new ref" << endl;
                    IpNodeReference * ipref = new IpNodeReference();
                    ipref->_ttl = 10;
                    ipref->_nodeId = receivedId.toString();

                    //
                    /*
                    uv_udp_init(_uvLoop, &ipref->_udpRecv);
                    struct sockaddr_in recv_addr;
                    recv_addr = uv_ip4_addr("0.0.0.0", recvPort);
                    uv_udp_bind( &ipref->_udpRecv, recv_addr,0);
                    uv_udp_recv_start(&ipref->_udpRecv, alloc_cb, node_recv_cb);
                    _nodeRefs.insert( make_pair(nodeIp, ipref) );
                    */
                    //

                }
                else
                {
                    cout << "existing ref" << endl;
                    _nodeRefs[ nodeIp ]->_ttl = 10;
                }
                _parentNode->addNeighbourReference( ShortId(_nodeRefs[ nodeIp ]->_nodeId), this );
            }
            else
            {
                cout << "received our own id" << endl;
            }
        }
    }

}

bool umn::ip::IpInterface::checkNetworkInterfaceExists( const std::string& ifName )
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

bool umn::ip::IpInterface::findInterfaceAddress(const string &ifName, string &ifAddr)
{
    char buf[512];
    uv_interface_address_t *info;
    int count, i;

    bool ret = false;

    uv_interface_addresses(&info, &count);
    i = count;

    // printf("Number of interfaces: %d\n", count);
    while (i--) {
        uv_interface_address_t interface = info[i];

        //        printf("Name: %s\n", interface.name);
        //        printf("Internal? %s\n", interface.is_internal ? "Yes" : "No");

        if (interface.address.address4.sin_family == AF_INET) {
            uv_ip4_name(&interface.address.address4, buf, sizeof(buf));
            std::string strName = interface.name;
            if( strName == ifName )
            {
                ret = true;
                ifAddr = buf;
                break;
            }
        }

        //        else if (interface.address.address4.sin_family == AF_INET6) {
        //            uv_ip6_name(&interface.address.address6, buf, sizeof(buf));
        //            printf("IPv6 address: %s\n", buf);
        //        }
    }

    uv_free_interface_addresses(info, count);
    return ret;
}

void umn::ip::IpInterface::startBeaconEmission()
{
    // Start emitting beacon on broadcast port on the right interface
}

void umn::ip::IpInterface::startBeaconReception()
{
    // Start receiving beacons on broadcast port on the right interface
    uv_udp_init( _uvLoop, &_beaconRecvSocket);
    struct sockaddr_in recv_addr;
    // uv_udp_set_multicast_interface()
    uv_ip4_addr("0.0.0.0", 6900, &recv_addr);
    uv_udp_bind(&_beaconRecvSocket, (const struct sockaddr*)&recv_addr,0);
    uv_udp_set_multicast_interface( &_beaconRecvSocket, _ownIpAddr.c_str());
    uv_udp_recv_start(&_beaconRecvSocket, alloc_cb, on_read);
}

void umn::ip::IpInterface::node_recv_cb(uv_udp_t *handle, ssize_t nread, uv_buf_t, sockaddr *addr, unsigned flags)
{
    IpInterface* ipi = (IpInterface*)handle->data;
    cout << "coucou ?" << endl;

}

void umn::ip::IpInterface::onReceiveBeacon(uv_udp_t *handle, ssize_t nread, uv_buf_t, sockaddr *addr, unsigned flags)
{


}
