#pragma once

#include "transport.h"

namespace umn
{
    class NNTx : public Transport
    {
    public:
        NNTx( const std::string& remoteUrl );
        virtual ~NNTx();
        virtual int fd();

        virtual int read( uint8_t* buffer, size_t len ){ return -1; }
        virtual int write( uint8_t* buffer, size_t len );

    private:

    protected:
        int _remotesock;
    };
}
