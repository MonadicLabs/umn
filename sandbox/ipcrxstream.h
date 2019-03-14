#pragma once

#include "stream.h"
#include <string>

namespace umn {
    class IPCRxStream : public Stream
    {
    public:
        IPCRxStream( const std::string& endpoint );
        virtual ~IPCRxStream();
        virtual size_t read( uint8_t* buffer, size_t buffer_size );

    private:
        int _sock;

    protected:

    };
}
