#pragma once

#include <iostream>
#include <iomanip>

using namespace std;

#include <cstring>

#define HEX( x ) setw(2) << setfill('0') << hex << (int)( x )

class Packet
{
public:

    typedef enum
    {
        UMN_BEACON_PKT = 0,

        UMN_LAST_PKT
    } Type;

    Packet();
    Packet( const Packet& other )
    {
        *this = other;
    }

    Packet( const Packet::Type& type );

    Packet& operator = (const Packet& other )
    {
        _type = other._type;
        _sendId = other._sendId;
        _recvId = other._recvId;
    }

    void setType( Packet::Type type )
    {
        _type = type;
    }

    void setSenderId( const uint32_t& sid )
    {
        _sendId = sid;
    }

    void setRecepientId( const uint32_t& rid )
    {
        _recvId = rid;
    }

    uint32_t getSenderId()
    {
        return _sendId;
    }

    uint32_t getRecipientId()
    {
        return _recvId;
    }

    int copyToBuffer( uint8_t* buffer, int bufferSize )
    {
        uint8_t* popo = buffer;
        int offset = 0;
        popo[ offset++ ] = 'U';
        popo[ offset++ ] = 'M';
        popo[ offset++ ] = 0;

        memcpy( popo + offset, &(_sendId), 4 );
        offset += 4;

        memcpy( popo + offset, &(_recvId), 4 );
        offset += 4;

        return offset;
    }

    void debugPrint()
    {
        unsigned char popo[1024];
        int r = copyToBuffer( popo, 1024 );
        cerr << "####" << endl;
        for( int k = 0; k < r; ++k )
        {
            cerr << "\\x" << HEX((int)(popo[k])) << "";
        }
        cerr << "####" << endl;

    }

private:
    Packet::Type _type;

    uint8_t _protocolMajorVersion;
    uint8_t _protocolMinorVersion;

    uint32_t _sendId;
    uint32_t _recvId;

};
