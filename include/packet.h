#pragma once

#include <sole/sole.hpp>

#include <iostream>
using namespace std;

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

    void setSenderId( const sole::uuid& sid )
    {
        _sendId = sid;
    }

    void setRecepientId( const sole::uuid& rid )
    {
        _recvId = rid;
    }

    sole::uuid getSenderId()
    {
        return _sendId;
    }

    int copyToBuffer( uint8_t* buffer, int bufferSize )
    {
        uint8_t* popo = buffer;
        int offset = 0;
        popo[ offset++ ] = 'U';
        popo[ offset++ ] = 'M';
        popo[ offset++ ] = 0;

        memcpy( popo + offset, &(_sendId.ab), 8 );
        offset += 8;
        memcpy( popo + offset, &(_sendId.cd), 8 );
        offset += 8;

        memcpy( popo + offset, &(_recvId.ab), 8 );
        offset += 8;
        memcpy( popo + offset, &(_recvId.cd), 8 );
        offset += 8;

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

    sole::uuid _sendId;
    sole::uuid _recvId;

};
