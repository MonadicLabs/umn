#pragma once

#include "utils.h"

namespace umn
{
    class FrameParser
    {
    public:
        FrameParser()
        {

        }

        virtual ~FrameParser()
        {

        }

        void reset()
        {

        }

        bool parse( uint8_t* data, size_t data_size )
        {
            UMN_DEBUG_PRINT( "Parsing !\n" );
            return false;
        }

    };
}
