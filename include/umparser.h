#pragma once

#include <deque>
#include <iostream>

#include <unistd.h>

#include "packet.h"

using namespace std;

#define DEBUG

class UMParser
{
public:
    typedef enum
    {
        UMPARSER_WAIT_SYNC_0 = 0,
        UMPARSER_WAIT_SYNC_1,
        UMPARSER_WAIT_PTYPE,
        UMPARSER_WAIT_SENDERID,
        UMPARSER_WAIT_RECVERID
    } State;

    UMParser()
        :_curState(UMPARSER_WAIT_SYNC_0)
    {
        _tmpBuffer = new unsigned char[1024];
    }

    virtual ~UMParser()
    {
        delete _tmpBuffer;
    }

    void appendByte( uint8_t* buffer, int offset )
    {

        uint8_t curByte = buffer[offset];
        State prevState = _curState;

        switch( _curState )
        {
        case UMPARSER_WAIT_SYNC_0:
        {
            if( curByte == 'U' )
            {
#ifdef DEBUG
                cerr << "found U" << endl;
#endif
                // Create a blank packet to start from
                _curPacket = Packet();
                _curState = UMPARSER_WAIT_SYNC_1;
            }
            break;
        }

        case UMPARSER_WAIT_SYNC_1:
        {
            if( curByte == 'M' )
            {
#ifdef DEBUG
                cerr << "found M" << endl;
#endif
                _curState = UMPARSER_WAIT_PTYPE;
            }
            else
            {
                _curState = UMPARSER_WAIT_SYNC_0;
            }
            break;
        }

        case UMPARSER_WAIT_PTYPE:
        {
            if( curByte < Packet::UMN_LAST_PKT )
            {
                _curPacket.setType( (Packet::Type)(curByte) );
                _curState = UMPARSER_WAIT_SENDERID;
                _remainingBytes = 16;
            }
            else
            {
                _curState = UMPARSER_WAIT_SYNC_0;
            }
            break;
        }

        case UMPARSER_WAIT_SENDERID:
        {
            _tmpBuffer[ 16 - _remainingBytes ] = curByte;
            _remainingBytes--;
            if( _remainingBytes == 0 )
            {
                uint64_t ab, cd;
                memcpy( &ab, _tmpBuffer, sizeof(uint64_t) );
                memcpy( &cd, _tmpBuffer + sizeof(uint64_t), sizeof(uint64_t) );
                sole::uuid sid = sole::rebuild(ab,cd);
                _curPacket.setSenderId(sid);
#ifdef DEBUG
                cerr << "received sender id: " << sid << endl;
#endif
                _curState = UMPARSER_WAIT_RECVERID;
                _remainingBytes = 16;
            }
            break;
        }

        case UMPARSER_WAIT_RECVERID:
        {
            _tmpBuffer[ 16 - _remainingBytes ] = curByte;
            _remainingBytes--;
            if( _remainingBytes == 0 )
            {
                uint64_t ab, cd;
                memcpy( &ab, _tmpBuffer, sizeof(uint64_t) );
                memcpy( &cd, _tmpBuffer + sizeof(uint64_t), sizeof(uint64_t) );
                sole::uuid sid = sole::rebuild(ab,cd);
                _curPacket.setRecepientId(sid);
#ifdef DEBUG
                cerr << "received receiver id: " << sid << endl;
#endif
                _packetQueue.push_back( _curPacket );
                _curState = UMPARSER_WAIT_SYNC_0;
            }
            break;
        }

        default:
            break;

        }
    }

    void appendData( uint8_t* buffer, int bufferSize )
    {
        for( int k = 0; k < bufferSize; ++k )
        {
            appendByte( buffer, k );
        }
    }

    bool getPacket( Packet& pkt )
    {
        if( _packetQueue.size() )
        {
            pkt = _packetQueue.front();
            _packetQueue.pop_front();
            return true;
        }
        return false;
    }

private:
    UMParser::State _curState;
    int _remainingBytes;
    uint8_t * _tmpBuffer;

    Packet _curPacket;
    std::deque<Packet> _packetQueue;

};
