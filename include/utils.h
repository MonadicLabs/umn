#pragma once

#include <string>

namespace novadem
{
namespace link
{
bool isMulticastAddress( const std::string& ipAddress );
std::string getNetworkInterfaceIP( const std::string& interfaceName );
unsigned long getMilliseconds();
void printHexMemory(void *mem, int mlen );
std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");
uint32_t intFromByteArray( uint8_t* buffer );

}
}
