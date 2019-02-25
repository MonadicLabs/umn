#pragma once

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
            printf( "parsing.\n");
            return false;
        }

    };
}
