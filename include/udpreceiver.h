#pragma once

#include <string>

namespace umn
{
    class UDPReceiver
    {
    public:
        UDPReceiver( uint16_t port, const std::string& networkInterface="", const std::string& multicastGroup="" );
        virtual ~UDPReceiver();

        virtual int receive(uint8_t *buffer, uint32_t bufferSize, std::string &senderIp);

    private:
        int _fd;
        uint16_t _port;
        std::string _address;
        std::string _networkInterface;

    protected:

    };
}
