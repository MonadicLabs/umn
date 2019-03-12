#pragma once

#include "utils.h"
#include "frame.h"

#include <arpa/inet.h>

namespace umn
{
class FrameParser
{
public:

    typedef enum
    {
        WAITING_START_SYMBOL = 0,
        WAITING_FLAGS,
        WAITING_TYPE,
        WAITING_SENDER_ID,
        WAITING_DEST_ID,
        WAITING_PAYLOAD_SIZE,
        WAITING_PAYLOAD,
        WAITING_CRC,
        WAITING_END_SYMBOL
    } State;

    FrameParser()
        :_curState( WAITING_START_SYMBOL ), _offset(0)
    {

    }

    virtual ~FrameParser()
    {

    }

    void reset()
    {
        _offset = 0;
        _curState = WAITING_START_SYMBOL;
        _itemOffset = 0;
    }

    bool parse( uint8_t* data, size_t data_size )
    {
        UMN_DEBUG_PRINT( "Parsing data_size=%d!\n", data_size );
        for( int i = 0; i < data_size; ++i )
        {
            if( parseByte( data[i] ) )
            {
                return true;
            }
        }
        return false;
    }

    // UGLY AF
    Frame _curFrame;

private:
    State _curState;
    uint8_t _tmp[ MAX_FRAME_BUFFER_SIZE ];
    int _offset;
    int _itemOffset;

    bool parseByte( uint8_t b )
    {
        bool ret = false;
        UMN_DEBUG_PRINT( "BYTE=%d! STATE=%d\n", (int)b, (int)_curState );
        switch( _curState )
        {
        case WAITING_START_SYMBOL:
        {
            if( b == FRAME_START_SYMBOL )
            {
                _curState = WAITING_FLAGS;
                _offset = 0;
                _itemOffset = 0;
                _tmp[ _offset++ ] = b;
                UMN_DEBUG_PRINT( "RECEIVED START SYMBOL\n" );
            }
            break;
        }

        case WAITING_FLAGS:
        {
            _curFrame.flags = b;
            _tmp[ _offset++ ] = b;
            _curState = WAITING_TYPE;
            UMN_DEBUG_PRINT( "RECEIVED FLAGS\n" );
            break;
        }

        case WAITING_TYPE:
        {
            _curFrame.type = b;
            _tmp[ _offset++ ] = b;
            _curState = WAITING_SENDER_ID;
            UMN_DEBUG_PRINT( "RECEIVED FRAME TYPE\n" );
            break;
        }

        case WAITING_SENDER_ID:
        {
            _tmp[ _offset++ ] = b;
            _itemOffset++;
            if( _itemOffset == sizeof(uint16_t) ) // Have we received the 2 bytes of our 16bit item ?
            {
                _curFrame.sender_id = ntohs(*(uint16_t*)(_tmp+_offset-sizeof(uint16_t)));
                _itemOffset = 0;
                _curState = WAITING_DEST_ID;
                UMN_DEBUG_PRINT( "RECEIVED SENDER_ID=%d\n", _curFrame.sender_id );
            }
            break;
        }

        case WAITING_DEST_ID:
        {
            _tmp[ _offset++ ] = b;
            _itemOffset++;
            if( _itemOffset == sizeof(uint16_t) ) // Have we received the 2 bytes of our 16bit item ?
            {
                _curFrame.dest_id = ntohs(*(uint16_t*)(_tmp+_offset-sizeof(uint16_t)));
                _itemOffset = 0;
                _curState = WAITING_PAYLOAD_SIZE;
                UMN_DEBUG_PRINT( "RECEIVED DEST_ID=%d\n", _curFrame.dest_id );
            }
            break;
        }

        case WAITING_PAYLOAD_SIZE:
        {
            _tmp[ _offset++ ] = b;
            _itemOffset++;
            if( _itemOffset == sizeof(uint16_t) ) // Have we received the 2 bytes of our 16bit item ?
            {
                _curFrame.payload_size = ntohs(*(uint16_t*)(_tmp+_offset-sizeof(uint16_t)));
                _itemOffset = 0;
                _curState = WAITING_PAYLOAD;
                UMN_DEBUG_PRINT( "RECEIVED PAYLOAD_SIZE=%d\n", _curFrame.payload_size );
            }
            break;
        }

        case WAITING_PAYLOAD:
        {
            _tmp[ _offset++ ] = b;
            _curFrame.payload[ _itemOffset++ ] = b;
            UMN_DEBUG_PRINT( "RECEIVED BYTE OF PAYLOAD _itemOffset=%d\n", _itemOffset );
            if( _itemOffset == _curFrame.payload_size ) // Have we received the N bytes of our item ?
            {
                _itemOffset = 0;
                _curState = WAITING_CRC;
                UMN_DEBUG_PRINT( "RECEIVED PAYLOAD\n" );
            }
            break;
        }

        case WAITING_CRC:
        {
            _tmp[ _offset++ ] = b;
            _itemOffset++;
            if( _itemOffset == sizeof(uint16_t) ) // Have we received the 2 bytes of our 16bit item ?
            {
                _curFrame.crc = ntohs(*(uint16_t*)(_tmp+_offset-sizeof(uint16_t)));
                _itemOffset = 0;
                _curState = WAITING_START_SYMBOL;
                // TODO: Check CRC validity by computing it on _tmp bytes
                bool frameValid = true;
                if( frameValid )
                {
                    _curState = WAITING_END_SYMBOL;
                }
            }
            break;
        }

        case WAITING_END_SYMBOL:
        {
            if( b == FRAME_END_SYMBOL )
            {
                // Done, we have a valid frame !
                ret = true;
            }
            _curState = WAITING_START_SYMBOL;
            break;
        }

        default:
            break;
        }

        return ret;
    }

};
}
