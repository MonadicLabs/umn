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

    void debugPrint()
    {
        unsigned char popo[1024];
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

        cerr << "####" << endl;
        for( int k = 0; k < offset; ++k )
        {
            cerr << "\\x" << HEX((int)(popo[k])) << "";
        }
        cerr << "####" << endl;

    }

private:
    Packet::Type _type;

    sole::uuid _sendId;
    sole::uuid _recvId;

};
