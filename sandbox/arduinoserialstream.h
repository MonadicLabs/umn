#pragma once

#include "stream.h"

namespace umn
{
    class ArduinoSerialStream : public umn::Stream
    {
    public:
        ArduinoSerialStream()
        {
            Serial1.begin(9600);
        }

        virtual ~ArduinoSerialStream()
        {

        }

        virtual size_t read(uint8_t *buffer, size_t buffer_size)
        {
            if( Serial1.available() )
            {
                int rval = Serial1.readBytes( buffer, buffer_size );
                if( rval > 0 )
                {
                    return rval;
                }
                else
                {
                    return 0;
                }
            }
        }

    private:

    protected:

    };
}
