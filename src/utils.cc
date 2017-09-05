
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
#include <iostream>

using namespace std;

#include <uv.h>

bool novadem::link::isMulticastAddress(const std::string &ipAddress)
{
    in_addr_t a = inet_addr( ipAddress.c_str() );
    //in_addr_t stored in network order
    uint32_t address = ntohl(a);
    return (address & 0xF0000000) == 0xE0000000;
}


std::string novadem::link::getNetworkInterfaceIP(const std::string &interfaceName)
{
    return getInterfaceIP(interfaceName);
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


std::string getInterfaceIP(const std::string &ifaceName)
{

    std::string ret = "";

    char buf[512];
    uv_interface_address_t *info;
    int count, i;

    uv_interface_addresses(&info, &count);
    i = count;

    // printf("Number of interfaces: %d\n", count);
    while (i--) {
        uv_interface_address_t interface_wlan = info[i];

        // printf("Name: %s\n", interface_wlan.name);
        // printf("Internal? %s\n", interface_wlan.is_internal ? "Yes" : "No");

        if (interface_wlan.address.address4.sin_family == AF_INET) {
            uv_ip4_name(&interface_wlan.address.address4, buf, sizeof(buf));
            // printf("IPv4 address: %s\n", buf);

            //
            // cerr << "l:" << std::string(interface_wlan.name) << " q:" << ifaceName << endl;
            if( std::string(interface_wlan.name) == ifaceName )
            {
                ret = std::string(buf);
            }
            //
        }
        else if (interface_wlan.address.address4.sin_family == AF_INET6) {
            uv_ip6_name(&interface_wlan.address.address6, buf, sizeof(buf));
            // printf("IPv6 address: %s\n", buf);
        }

        // printf("\n");
    }

    uv_free_interface_addresses(info, count);
    // cerr << ifaceName << " ret=" << ret << endl;
    return ret;
}
