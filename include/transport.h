#pragma once

#include <cstdint>
#include <cstddef>

namespace umn
{
    class Transport
    {
    public:
        Transport();
        virtual ~Transport();

        virtual int fd(){ return -1; }
        virtual int read( uint8_t* buffer, size_t len ){ return -1; }
        virtual int write( uint8_t* buffer, size_t len ){ return -1; }

    };
}
