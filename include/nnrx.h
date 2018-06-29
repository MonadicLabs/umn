#pragma once

#include "transport.h"

namespace umn
{
    class NNRx : public Transport
    {
    public:
        NNRx( const std::string& localUrl );
        virtual ~NNRx();
        virtual int fd();

        virtual int read( uint8_t* buffer, size_t len );
        virtual int write( uint8_t* buffer, size_t len ){ return -1; }

    private:

    protected:
        int _localsock;
        int _pollable;

    };
}
