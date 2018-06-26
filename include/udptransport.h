#pragma once

#include <string>

#include "transport.h"

namespace umn
{
class UDPTransport : public Transport
{
public:
    UDPTransport( const std::string remoteHost,
                  uint16_t remotePort,
                  uint16_t localPort )
        :_txHost(remoteHost), _port(remotePort), _localPort(localPort)
    {
        init_rx();
        init_tx();
    }

    virtual ~UDPTransport()
    {

    }

    virtual int fd()
    {
        return _rxfd;
    }

    virtual int read(uint8_t *buffer, size_t len);
    virtual int write(uint8_t *buffer, size_t len);

private:

    int _rxfd;

    int _txfd;
    void* _groupSock;

    std::string _txHost;
    uint16_t _port;
    uint16_t _localPort;

    void init_rx();
    void init_tx();

};
}
