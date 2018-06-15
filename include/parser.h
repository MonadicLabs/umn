#pragma once

#include <memory>
#include <iostream>
#include <deque>
using namespace std;

#include "frame.h"

namespace umn
{
class Parser
{
public:

    typedef enum
    {
        WAITING_FOR_START = 0,
        WAITING_FOR_START_2,
        WAITING_FOR_SYNC,
        WAITING_FOR_VERSION,
        WAITING_FOR_TYPE,
        WAITING_FOR_FLAGS,
        WAITING_FOR_SENDER,
        WAITING_FOR_RECIPIENT,
        WAITING_FOR_PREV_HOP,
        WAITING_FOR_ID,
        WAITING_FOR_FRAME_LEN,
        WAITING_FOR_DATA,
        WAITING_FOR_CRC
    } State;

    Parser()
    {
        reset();
    }

    virtual ~Parser()
    {

    }

    bool parse( uint8_t* data, size_t len )
    {
        for( int k = 0; k < len; ++k )
        {
            if( parse( data[k] ) )
            {

            }
        }
        return hasNext();
    }

    bool parse( uint8_t b )
    {
        switch( _state )
        {
        case WAITING_FOR_START:
        {
            // cerr << "waiting for start_0" << endl;
            if( b == Frame::START_0_SYMBOL )
                _state = WAITING_FOR_START_2;
            break;
        }
        case WAITING_FOR_START_2:
        {
            // cerr << "waiting for start_2" << endl;
            if( b == Frame::START_1_SYMBOL )
                _state = WAITING_FOR_SYNC;
            else
                reset();
            break;
        }
        case WAITING_FOR_SYNC:
        {
            // cerr << "waiting for sync" << endl;
            if( b == Frame::SYNC_SYMBOL )
                _state = WAITING_FOR_VERSION;
            else
                reset();
            break;
        }
        case WAITING_FOR_VERSION:
        {
            // cerr << "waiting for version" << endl;
            if( b == Frame::VERSION )
            {
                _state = WAITING_FOR_TYPE;
            }
            else
                reset();
            break;
        }
        case WAITING_FOR_TYPE:
        {
            if( b >= Frame::TYPE_END )
            {
                reset();
            }
            else
            {
                _curFrame->_type = (Frame::Type)b;
                _state = WAITING_FOR_FLAGS;
            }
            break;
        }
        case WAITING_FOR_FLAGS:
        {
            _curFrame->_flags = b;
            _state = WAITING_FOR_SENDER;
            break;
        }
        case WAITING_FOR_SENDER:
        {
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == 4 )
            {
                _curFrame->_sender = NodeAddress::fromMemory( _tmpBuffer, 4 );
                _state = WAITING_FOR_RECIPIENT;
                _curOffset = 0;
            }
            break;
        }
        case WAITING_FOR_RECIPIENT:
        {
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == 4 )
            {
                _curFrame->_destination = NodeAddress::fromMemory( _tmpBuffer, 4 );
                _state = WAITING_FOR_PREV_HOP;
                _curOffset = 0;
            }
            break;
        }
        case WAITING_FOR_PREV_HOP:
        {
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == 4 )
            {
                _curFrame->_previousHop = NodeAddress::fromMemory( _tmpBuffer, 4 );
                _state = WAITING_FOR_ID;
                _curOffset = 0;
            }
            break;
        }
        case WAITING_FOR_ID:
        {
            // cerr << "[ID] curoffset=" << _curOffset << " b=" << std::hex << (int)b << endl;
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == 2 )
            {
                _curFrame->_id = _tmpBuffer[1] | _tmpBuffer[0] << 8;
                _state = WAITING_FOR_FRAME_LEN;
                _curOffset = 0;
            }
            break;
        }
        case WAITING_FOR_FRAME_LEN:
        {
            // cerr << "curoffset=" << _curOffset << " b=" << std::hex << (int)b << endl;
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == 2 )
            {
                _curFrame->_payloadLength = _tmpBuffer[1] | _tmpBuffer[0] << 8;
                // cerr << "PAYLOAD_LENGTH = " << _curFrame->_payloadLength << endl;
                _state = WAITING_FOR_DATA;
                _curOffset = 0;
            }
            break;
        }
        case WAITING_FOR_DATA:
        {
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == _curFrame->_payloadLength )
            {
                _curFrame->setPayload( _tmpBuffer, _curFrame->_payloadLength );
                _state = WAITING_FOR_CRC;
                _curOffset = 0;
            }
            break;
        }
        case WAITING_FOR_CRC:
        {
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == 2 )
            {
                _curFrame->_crc = _tmpBuffer[1] | _tmpBuffer[0] << 8;
                _state = WAITING_FOR_START;
                _frames.push_back( _curFrame );
                reset();
                // cerr << "ok done. crc=" << _curFrame->_crc << endl;
                return true;
            }
            break;
        }
        default:
            break;
        }
        return false;
    }

    bool hasNext()
    {
        return _frames.size() > 0;
    }

    std::shared_ptr<Frame> next()
    {
        std::shared_ptr< Frame > f = _frames.front();
        _frames.pop_front();
        return f;
    }

private:
    State _state;
    int _remainingDataToRead;
    int _curOffset;

    std::shared_ptr< Frame > _curFrame;

    uint8_t _tmpBuffer[ 16384 ]; // MAX_UMN_PAYLOAD_MTU * 2 = 8192 * 2

    std::deque< std::shared_ptr< Frame > > _frames;

    void reset()
    {
        _state = WAITING_FOR_START;
        _curOffset = 0;
        _curFrame = std::make_shared< Frame >();
    }

protected:

};
}
