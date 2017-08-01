
#include "utils.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <string>
#include <algorithm>
#include <chrono>

bool novadem::link::isMulticastAddress(const std::string &ipAddress)
{
    in_addr_t a = inet_addr( ipAddress.c_str() );
    //in_addr_t stored in network order
    uint32_t address = ntohl(a);
    return (address & 0xF0000000) == 0xE0000000;
}


std::string novadem::link::getNetworkInterfaceIP(const std::string &interfaceName)
{
    std::string ret = "";
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];
    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if((strcmp(ifa->ifa_name, interfaceName.c_str())==0)&&(ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
#ifdef USE_LOGGING
                LOG_S(ERROR) << "getnameinfo failed !";
#endif
                exit(EXIT_FAILURE);
            }
            // printf("\tInterface : <%s>\n",ifa->ifa_name );
            // printf("\t  Address : <%s>\n", host);
            ret = host;
        }
    }
    freeifaddrs(ifaddr);
    return ret;
}


unsigned long novadem::link::getMilliseconds()
{

    unsigned long milliseconds_since_epoch =
            std::chrono::steady_clock::now().time_since_epoch() /
            std::chrono::milliseconds(1);
    return milliseconds_since_epoch;

}


void novadem::link::printHexMemory(void *mem, int mlen)
{
    int i;
    unsigned char *p = (unsigned char *)mem;
    for (i=0;i<mlen;i++) {
        printf("0x%02x ", p[i]);
        if (i%16==0 && i>0)
            printf("\n");
    }
    printf("\n");
}

// trim from left
std::string& novadem::link::ltrim(std::string& s, const char* t )
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}



uint32_t novadem::link::intFromByteArray(uint8_t *buffer)
{
    uint32_t ret = 0;
    memcpy( &ret, buffer, sizeof(uint32_t) );
    return ret;
}
