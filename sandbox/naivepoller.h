#pragma once

#include "poller.h"
#include "stream.h"

namespace umn
{
    class UMN;
    class NaivePoller : public Poller
    {
    public:
        NaivePoller()
        {

        }

        virtual ~NaivePoller()
        {

        }

        virtual void poll(Stream ** streams, int num_streams)
        {
            uint8_t buffer[8192];
            for( int i = 0; i < num_streams; ++i )
            {
                if( streams[i] != 0 )
                {
                    size_t r_val = streams[i]->read(buffer, 8192);
                    if( r_val > 0 )
                    {
                        if( streams[i]->_ctx )
                        {
                            UMN_DEBUG_PRINT( "r_val=%d\n", r_val );
                            UMN* uctx = (UMN*)(streams[i]->_ctx);
                            uctx->onNewData( streams[i], buffer, r_val );
                        }
                    }
                }
            }
        }

    private:

    protected:

    };
}
