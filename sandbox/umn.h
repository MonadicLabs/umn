#pragma once

#define UMN_MAX_STREAM_NUMBER 256
// #define DEBUG 1

#include "stream.h"
#include "poller.h"
#include "frame.h"
#include "frameparser.h"
#include "router.h"

#include <iostream>
using namespace std;

#include <unistd.h>

namespace umn
{
    class UMN
    {
    public:
        UMN( Poller* poller = 0 )
            :_poller(poller), _currentNumStreams(0), _id(0x0001)
        {
            for( int i = 0; i < UMN_MAX_STREAM_NUMBER; ++i )
            {
                _streams[i] = 0;
            }
            _router = new Router( this );
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

            if( _router )
            {
                _router->tick();
                usleep(10000);
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
            UMN_DEBUG_PRINT("sidx=%d\n", sidx);
            if( sidx >= 0 )
            {
                if( _parsers[sidx].parse( buffer, data_size ) )
                {
                    onNewFrame( s, _parsers[sidx]._curFrame );
                }
            }
        }

        void onNewFrame( Stream* s, Frame& f )
        {
            UMN_DEBUG_PRINT( "UMN NEW FRAME\n" );
            // if( f.sender_id != _id )
            {
                if( _router )
                {
                    _router->onNewFrame( s, f );
                }
            }
            /*
            else
            {
                UMN_DEBUG_PRINT( "RECEIVED OWN FRAME\n" );
            }
            */
        }

        void broadcast( Frame& f )
        {
            uint8_t buffer[ MAX_FRAME_BUFFER_SIZE ];
            int fsize = frameToBuffer( f, buffer, MAX_FRAME_BUFFER_SIZE );
            // print_bytes( cerr, "frame", buffer, fsize );
            for( int k = 0; k < UMN_MAX_STREAM_NUMBER; ++k )
            {
                Stream * s = _streams[k];
                if( s )
                {
                    s->write( buffer, fsize );
                }
            }
            //
        }

        uint16_t id()
        {
            return _id;
        }

    private:
        Stream * _streams[ UMN_MAX_STREAM_NUMBER ];
        FrameParser _parsers[ UMN_MAX_STREAM_NUMBER ];
        int _currentNumStreams;
        Poller * _poller;
        Router * _router;
        uint16_t _id;

    };
}
