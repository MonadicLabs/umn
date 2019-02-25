#pragma once

#define UMN_MAX_STREAM_NUMBER 256


#include "stream.h"
#include "poller.h"
#include "frame.h"
#include "frameparser.h"

namespace umn
{
    class UMN
    {
    public:
        UMN( Poller* poller = 0 )
            :_poller(poller), _currentNumStreams(0)
        {
            for( int i = 0; i < UMN_MAX_STREAM_NUMBER; ++i )
            {
                _streams[i] = 0;
            }
        }

        virtual ~UMN()
        {

        }

        void tick()
        {
            if( _poller )
            {
                _poller->poll( _streams, UMN_MAX_STREAM_NUMBER );
            }
        }

        void addStream( Stream * s )
        {
            s->_ctx = (void*)this;
            _streams[ _currentNumStreams++ ] = s;
            _parsers[ _currentNumStreams ].reset();
        }

        void onNewData( Stream* s, uint8_t * buffer, size_t data_size )
        {
            // Search for stream idx
            int sidx = -1;
            for( int  i = 0; i < UMN_MAX_STREAM_NUMBER; ++i )
            {
                if( _streams[i] == s )
                {
                    sidx = i;
                    break;
                }
            }
            if( sidx >= 0 )
            {
                _parsers[sidx].parse( buffer, data_size );
            }
        }

        void onNewFrame( Stream* s, Frame& f )
        {

        }

    private:
        Stream * _streams[ UMN_MAX_STREAM_NUMBER ];
        FrameParser _parsers[ UMN_MAX_STREAM_NUMBER ];
        int _currentNumStreams;
        Poller * _poller;

    };
}
