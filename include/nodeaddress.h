#pragma once

#include "utils.h"

#include <string.h>

namespace umn
{
    class NodeAddress
    {
    public:

        NodeAddress(){}
        virtual ~NodeAddress(){}

        NodeAddress( const NodeAddress& other )
        {
            *this = other;
        }

        NodeAddress& operator = (const NodeAddress& other)
        {
            _intValue = other._intValue;
            return *this;
        }

        bool operator <( NodeAddress const& rhs ) const {
            return _intValue < rhs._intValue;
        }

        bool operator == ( NodeAddress const& rhs )
        {
            return _intValue == rhs._intValue;
        }

        static NodeAddress fromMemory( uint8_t* buffer, size_t len )
        {
            NodeAddress na;
            na._intValue = buffer[3] | buffer[2] << 8 | buffer[1] << 16 | buffer[0] << 24;
            return na;
        }

        static NodeAddress fromInteger( uint32_t value )
        {
            NodeAddress na;
            na._intValue = value;
            return na;
        }

        void copyTo( uint8_t* buffer )
        {
            uint32_t sv = bs32(_intValue);
            memcpy( buffer, (uint8_t*)(&sv), sizeof(uint32_t) );
        }

        uint32_t asInteger()
        {
            return _intValue;
        }

    private:
        uint32_t _intValue;

    protected:

    };
}
