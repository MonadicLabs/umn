#pragma once

#include <deque>
#include <queue>

// #define PARSER_DEBUG

namespace novadem
{
class SBFPPacket
{
public:
    SBFPPacket()
        :datalen(0), dataLeft(0), dataOffset(0)
    {}

    SBFPPacket( const SBFPPacket& other )
    {
        *this = other;
    }

    virtual ~SBFPPacket(){}

    SBFPPacket& operator =( const SBFPPacket& other )
    {
        sync1 = other.sync1;
        sync2 = other.sync2;
        flags = other.flags;
        framenumber = other.framenumber;
        datalen = other.datalen;
        dataLeft = other.dataLeft;
        dataOffset = other.dataOffset;
        memcpy( data, other.data, 256 );
        cka = other.cka;
        ckb = other.ckb;
    }

    int toBuffer( unsigned char* buffer )
    {
        int curOffset = 0;
        buffer[curOffset++] = sync1;
        buffer[curOffset++] = sync2;
        buffer[curOffset++] = flags;
        buffer[curOffset++] = framenumber;
        buffer[curOffset++] = datalen;
        for( size_t k = 0; k < datalen; ++k )
        {
            buffer[curOffset] = data[k];
            curOffset++;
        }
        buffer[curOffset++] = cka;
        buffer[curOffset++] = ckb;
        return curOffset;
    }

    unsigned char sync1;
    unsigned char sync2;
    unsigned char flags;
    unsigned char framenumber;
    unsigned char datalen;
    unsigned char dataLeft;
    int dataOffset;
    unsigned char data[256];
    unsigned char cka;
    unsigned char ckb;
};

class SBFPParser
{
public:

    typedef enum
    {
        WAITING_FOR_SYNC = 0,
        WAITING_FOR_PROTOCOL,
        WAITING_FOR_FLAGS,
        WAITING_FOR_FRAME_NUMBER,
        WAITING_FOR_DATA_LEN,
        WAITING_FOR_DATA,
        WAITING_FOR_CKA,
        WAITING_FOR_CKB
    } State;

    SBFPParser()
        :_curState(WAITING_FOR_SYNC)
    {

    }

    virtual ~SBFPParser(){}

    void appendByte( unsigned char* buffer, int idx )
    {
        unsigned char curByte = buffer[idx];
        State prevState = _curState;
        switch( _curState )
        {
        case WAITING_FOR_SYNC:
        {
            if( curByte == 'N' )
            {
                // Create a blank packet to start from
                _curPacket = SBFPPacket();
                _curPacket.sync1 = curByte;
                _curState = WAITING_FOR_PROTOCOL;
            }
            break;
        }

        case WAITING_FOR_PROTOCOL:
        {
            if( curByte == 'U' )
            {
                _curPacket.sync2 = curByte;
                _curState = WAITING_FOR_FLAGS;
            }
            else
            {
                _curState = WAITING_FOR_SYNC;
            }
            break;
        }

        case WAITING_FOR_FLAGS:
        {
            _curPacket.flags = curByte;
            _curState = WAITING_FOR_FRAME_NUMBER;
            break;
        }

        case WAITING_FOR_FRAME_NUMBER:
        {
            _curPacket.framenumber = curByte;
            _curState = WAITING_FOR_DATA_LEN;
            break;
        }

        case WAITING_FOR_DATA_LEN:
        {
            _curPacket.datalen = curByte;
            _curPacket.dataLeft = _curPacket.datalen;
#ifdef PARSER_DEBUG
            std::cout << "datalen=" << (int)(_curPacket.datalen) << std::endl;
#endif
            _curState = WAITING_FOR_DATA;
            _curPacket.dataOffset = 0;
            break;
        }

        case WAITING_FOR_DATA:
        {
            if( _curPacket.dataLeft > 0 )
            {
#ifdef PARSER_DEBUG
                std::cout << "DATA - Got byte " << std::hex << (int)curByte << std::dec << " bytes left: " <<  _curPacket.dataLeft - 1 << std::endl;
#endif
                _curPacket.data[_curPacket.dataOffset++] = curByte;
                _curPacket.dataLeft--;
            }
            if( _curPacket.dataLeft <= 0 )
            {
                _curState = WAITING_FOR_CKA;
            }
            break;
        }

        case WAITING_FOR_CKA:
        {
            _curPacket.cka = curByte;
#ifdef PARSER_DEBUG
            std::cout << "CKA - Got byte " << std::hex << (int)curByte << std::dec << std::endl;
#endif
            _curState = WAITING_FOR_CKB;
            break;
        }

        case WAITING_FOR_CKB:
        {
            _curPacket.ckb = curByte;
            //_curState = curByte;
#ifdef PARSER_DEBUG
            std::cout << "CKB - Got byte " << std::hex << (int)curByte << std::dec << std::endl;
#endif
            _pktQueue.push_back( _curPacket );
            _curPacket = SBFPPacket();
            _curState = WAITING_FOR_SYNC;
            break;
        }

        }
#ifdef PARSER_DEBUG
        if( _curState != prevState )
        {
            std::cout << "Parser: switched from state " << prevState << " to state " << _curState << std::endl;
        }
#endif
    }

    void appendData( unsigned char* buffer, size_t bufferSize )
    {
        for( size_t k = 0; k < bufferSize; ++k )
        {
#ifdef PARSER_DEBUG
            std::cout << "append byte buffer[" << k << "] = " << std::hex << (int)(buffer[k]) << std::dec << std::endl;
#endif
            appendByte( buffer, k );
        }
    }

    std::deque<SBFPPacket> _pktQueue;

private:
    State _curState;
    SBFPPacket _curPacket;

protected:

};
}
