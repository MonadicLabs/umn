#pragma once

#include <memory>
#include <iostream>
#include <deque>
using namespace std;

#include "frame.h"

// #define PARSER_DEBUG

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
        WAITING_FOR_NUM_HOPS,
        WAITING_FOR_SEQ_NUM,
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
#ifdef PARSER_DEBUG
        cerr << "~ PARSER - CURRENT_BYTE=" << (int)b << endl;
#endif
        switch( _state )
        {
        case WAITING_FOR_START:
        {
#ifdef PARSER_DEBUG
            cerr << "waiting for start_0" << endl;
#endif
            if( b == Frame::START_0_SYMBOL )
                _state = WAITING_FOR_START_2;
            break;
        }
        case WAITING_FOR_START_2:
        {
#ifdef PARSER_DEBUG
            cerr << "waiting for start_2" << endl;
#endif
            if( b == Frame::START_1_SYMBOL )
                _state = WAITING_FOR_SYNC;
            else
                reset();
            break;
        }
        case WAITING_FOR_SYNC:
        {
#ifdef PARSER_DEBUG
            cerr << "waiting for sync" << endl;
#endif
            if( b == Frame::SYNC_SYMBOL )
                _state = WAITING_FOR_VERSION;
            else
                reset();
            break;
        }
        case WAITING_FOR_VERSION:
        {
#ifdef PARSER_DEBUG
            cerr << "waiting for version" << endl;
#endif
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
#ifdef PARSER_DEBUG
            cerr << "waiting for type" << endl;
#endif
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
#ifdef PARSER_DEBUG
            cerr << "waiting for flags" << endl;
#endif
            _curFrame->_flags = b;
            _state = WAITING_FOR_SENDER;
            _curOffset = 0;
            break;
        }
        case WAITING_FOR_SENDER:
        {
#ifdef PARSER_DEBUG
            cerr << "waiting for sender" << " - curbyte:" << (int)b << endl;
#endif
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
#ifdef PARSER_DEBUG
            cerr << "waiting for destination" << endl;
#endif
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == 4 )
            {
                _curFrame->_destination = NodeAddress::fromMemory( _tmpBuffer, 4 );
                _state = WAITING_FOR_NUM_HOPS;
                _curOffset = 0;
            }
            break;
        }
        case WAITING_FOR_NUM_HOPS:
        {
#ifdef PARSER_DEBUG
            cerr << "waiting for num_hops" << endl;
#endif
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == 2 )
            {
                _curFrame->_hops = _tmpBuffer[1] | _tmpBuffer[0] << 8;
                _state = WAITING_FOR_SEQ_NUM;
                _curOffset = 0;
            }
            break;
        }
        case WAITING_FOR_SEQ_NUM:
        {
#ifdef PARSER_DEBUG
            cerr << "waiting for seq_num" << endl;
#endif
            // cerr << "[ID] curoffset=" << _curOffset << " b=" << std::hex << (int)b << endl;
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == 2 )
            {
                _curFrame->_seqNum = _tmpBuffer[1] | _tmpBuffer[0] << 8;
                _state = WAITING_FOR_FRAME_LEN;
                _curOffset = 0;
            }
            break;
        }
        case WAITING_FOR_FRAME_LEN:
        {
#ifdef PARSER_DEBUG
            cerr << "waiting for payload_len" << endl;
#endif
            // cerr << "curoffset=" << _curOffset << " b=" << std::hex << (int)b << endl;
            _tmpBuffer[ _curOffset++ ] = b;
            if( _curOffset == 2 )
            {
                _curFrame->_payloadLength = _tmpBuffer[1] | _tmpBuffer[0] << 8;
                if( _curFrame->_payloadLength <= 0 )
                {
                    _state = WAITING_FOR_CRC;
                }
                else
                {
                    _state = WAITING_FOR_DATA;
                }
                _curOffset = 0;
            }
            break;
        }
        case WAITING_FOR_DATA:
        {
#ifdef PARSER_DEBUG
            cerr << "waiting for data expected_len=" << _curFrame->_payloadLength << endl;
#endif
            {
                _tmpBuffer[ _curOffset++ ] = b;
                if( _curOffset == _curFrame->_payloadLength )
                {
                    _curFrame->setPayload( _tmpBuffer, _curFrame->_payloadLength );
                    _state = WAITING_FOR_CRC;
                    _curOffset = 0;
                }
            }
            break;
        }
        case WAITING_FOR_CRC:
        {
#ifdef PARSER_DEBUG
            cerr << "waiting for crc" << endl;
#endif
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
        if( _frames.size() > 0 )
        {
            std::shared_ptr< Frame > f = _frames.front();
            _frames.pop_front();
            return f;
        }
        else
        {
            return nullptr;
        }
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
