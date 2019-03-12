#pragma once

#include "stream.h"
#include "s2string.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

namespace umn
{
class UDPStream : public Stream
{
public:
    UDPStream( uint16_t port, const s2::string& networkInterface, const s2::string& multicastGroup )
        :_port(port)
    {
        if (( _fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        {
            printf("Error opening datagram socket.\n");
        }

        {
            int reuse = 1;
            if(setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
            {
                printf("Setting SO_REUSEADDR...ERROR.\n");
                close(_fd);
            }
            else
            {
                printf("Setting SO_REUSEADDR...OK.\n");
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
                printf("Binding datagram socket ERROR\n");
                close(_fd);
            }
            else
            {
                printf("Binding datagram socket...OK.\n");
            }

            struct timeval read_timeout;
            read_timeout.tv_sec = 0;
            read_timeout.tv_usec = 10;
            setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

        }

    }

    virtual ~UDPStream()
    {

    }

    virtual size_t read(uint8_t *buffer, size_t buffer_size)
    {
        struct sockaddr_in si_other;
        int slen=sizeof(si_other);
        int rlen = recvfrom( _fd, buffer, buffer_size, 0, (sockaddr*)&si_other, &slen );
        // printf( "rlen=%d\n", rlen );
        if( rlen > 0 )
        {
            return rlen;
        }
        else
        {
            return 0;
        }
    }

    virtual size_t write(uint8_t* buffer, size_t buffer_size)
    {
        struct sockaddr_in groupSock;

        int s, i, slen=sizeof(groupSock);
        char buf[8192];

        memset((char *) &groupSock, 0, sizeof(groupSock));
        groupSock.sin_family = AF_INET;

        // if( isMulticastAddress( _host ) )
        std::string _host = "127.0.0.1";
        groupSock.sin_addr.s_addr = inet_addr( _host.c_str() );
        groupSock.sin_port = htons(_port);
        if (inet_aton(_host.c_str(), &groupSock.sin_addr)==0) {
            printf("inet_aton failed\n");
        }

//        if( buffer_size > UDP_MTU )
//        {

//        }

        if (sendto(_fd, buffer, buffer_size, 0, (const struct sockaddr*)(&groupSock), slen)==-1)
        {

        }
    }

private:
    uint16_t _port;
    s2::string _networkInterface;
    int _fd;

protected:

};
}
