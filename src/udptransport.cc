#include "udptransport.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int umn::UDPTransport::read(uint8_t *buffer, size_t len)
{
    int rlen = recv( _rxfd, buffer, len, 0 );
    return rlen;
}

int umn::UDPTransport::write(uint8_t *buffer, size_t len)
{
    if (sendto(_txfd, buffer, len, 0, (const struct sockaddr*)(_groupSock), sizeof(struct sockaddr))==-1)
    {

    }
}

void umn::UDPTransport::init_rx()
{
    if (( _rxfd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        {
    #ifdef USE_LOGGING
            LOG_S(ERROR) << "Error opening datagram socket.";
    #endif
        }

        {
            int reuse = 1;
            if(setsockopt(_rxfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
            {
    #ifdef USE_LOGGING
                LOG_S(INFO) << "Setting SO_REUSEADDR...ERROR.";
    #endif
                close(_rxfd);
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
            if(bind(_rxfd, (struct sockaddr*)&localSock, sizeof(localSock)))
            {
    #ifdef USE_LOGGING
                LOG_S(ERROR) << "Binding datagram socket ERROR";
    #endif
                close(_rxfd);
            }
            else
            {
    #ifdef USE_LOGGING
                LOG_S(INFO) << "Binding datagram socket...OK.";
    #endif
            }

        }

}

void umn::UDPTransport::init_tx()
{
    if (( _txfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    {
        // ERROR
    }
    else
    {
        struct sockaddr_in * groupSock = (struct sockaddr_in*)malloc( sizeof( struct sockaddr_in ) );
        int s, i, slen=sizeof(*groupSock);
        char buf[8192];
        memset((char *)groupSock, 0, sizeof(*groupSock));
        groupSock->sin_family = AF_INET;
        groupSock->sin_addr.s_addr = inet_addr( _txHost.c_str() );
        groupSock->sin_port = htons(_port);
        if (inet_aton(_txHost.c_str(), &(groupSock->sin_addr) )==0) {
            // ERROR
        }
        _groupSock = groupSock;
    }
}
