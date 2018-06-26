#pragma once

#include <cstdint>
#include <cstddef>

#include <memory>

#include "frame.h"

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
        virtual bool write( std::shared_ptr<Frame> f )
        {
            int r = write( f->buffer_ptr(), f->getBufferLength() );
            if( r == f->getBufferLength() )
            {
                return true;
            }
            else
            {
                return false;
            }
        }

    };
}
