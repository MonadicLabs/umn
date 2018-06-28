#pragma once

#include "transport.h"

namespace umn
{
    class NNPairTransport : public Transport
    {
    public:
        NNPairTransport( const std::string& url );
        virtual ~NNPairTransport();
        virtual int fd();

        virtual int read( uint8_t* buffer, size_t len );
        virtual int write( uint8_t* buffer, size_t len );

    private:

    protected:
        int _pairfd;
        int _sock;

    };
}
