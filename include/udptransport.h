#pragma once

#include <string>

#include "transport.h"

namespace umn
{
class UDPTransport : public Transport
{
public:
    UDPTransport( const std::string txHost,
                  uint16_t port )
        :_txHost(txHost), _port(port)
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

    void init_rx();
    void init_tx();

};
}
