#pragma once

#include "stream.h"
#include <string>

namespace umn
{
    class IPCTxStream : public Stream
    {
    public:
        IPCTxStream( const std::string& endpoint = "ipc:///tmp/polbak.ipc" );
        virtual ~IPCTxStream();
        virtual size_t write( uint8_t* buffer, size_t buffer_size );

    private:
        int _sock;

    protected:

    };
}
