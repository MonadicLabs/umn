
#include "udpreceiver.h"
#include "utils.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#include <iostream>

using namespace std;

// #undef USE_LOGGING

umn::UDPReceiver::UDPReceiver( uint16_t port, const std::string& networkInterface, const std::string& multicastGroup )
    :_address(multicastGroup), _port(port), _networkInterface(networkInterface)
{
    if (( _fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    {
#ifdef USE_LOGGING
      LOG_S(ERROR) << "Error opening datagram socket.";
#endif
    }

    {
        int reuse = 1;
        if(setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
        {
#ifdef USE_LOGGING
            LOG_S(INFO) << "Setting SO_REUSEADDR...ERROR.";
#endif
            close(_fd);
        }
        else
        {
#ifdef USE_LOGGING
            LOG_S(INFO) << "Setting SO_REUSEADDR...OK.";
#endif
        }

        /* Bind to the proper port number with the IP address */
        /* specified as INADDR_ANY. */
        struct sockaddr_in localSock;
        memset((char *) &localSock, 0, sizeof(localSock));
        localSock.sin_family = AF_INET;
        localSock.sin_port = htons(_port);
        localSock.sin_addr.s_addr = INADDR_ANY;
        if(bind(_fd, (struct sockaddr*)&localSock, sizeof(localSock)))
        {
#ifdef USE_LOGGING
            LOG_S(ERROR) << "Binding datagram socket ERROR";
#endif
            close(_fd);
        }
        else
        {
#ifdef USE_LOGGING
            LOG_S(INFO) << "Binding datagram socket...OK.";
#endif
        }

        // Joining multicast group if needed...
        if( isMulticastAddress( _address ) && _address.size() && _networkInterface.size() )
        {
            struct ip_mreq group;
            group.imr_multiaddr.s_addr = inet_addr( _address.c_str() );
            group.imr_interface.s_addr = inet_addr( getNetworkInterfaceIP(_networkInterface).c_str() );
            if(setsockopt( _fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
            {
#ifdef USE_LOGGING
                LOG_S(ERROR) << "ERROR adding multicast group";
#endif
                close(_fd);
            }
            else
            {
#ifdef USE_LOGGING
                LOG_S(INFO) << "Adding multicast group " << _address << " ...OK.";
#endif
            }
        }
    }

}

umn::UDPReceiver::~UDPReceiver()
{

}

int umn::UDPReceiver::receive(uint8_t *buffer, uint32_t bufferSize, std::string& senderIp )
{
    char buf[ 8192 ];
    struct sockaddr_in si_other;
    int slen=sizeof(si_other);
    socklen_t slen_other;

    int rlen = recvfrom( _fd, buf, 8192, 0, (struct sockaddr*)(&si_other), &slen_other );

    if( rlen > 0 )
    {
        memcpy( buffer, buf, rlen );
        return rlen;
    }
    return -1;

}
