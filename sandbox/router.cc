
#include "router.h"
#include "umn.h"

#include <cstring>

void umn::Router::broadcastRoutingTable()
{
    Frame rtframe = createRoutingTableFrame();
    if( _parent )
    {
        UMN_DEBUG_PRINT( "broadcast ?\n" );
        _parent->broadcast( rtframe );
    }
}

umn::Frame umn::Router::createRoutingTableFrame()
{
    Frame f;
    int offset = 0;

    // Write number of entries (MAX 255, then)
    f.payload[ offset++ ] = (uint8_t)(_reverseRoutes.size());
    for( int k = 0; k < _reverseRoutes.size(); ++k )
    {
        DistanceVector dv = _reverseRoutes[k];
//        dv.source.copyTo( (uint8_t*)(buffer + offset) );
        uint16_t nsource = htons(dv.source);
        memcpy( f.payload + offset, &nsource, sizeof(uint16_t) );
        offset += sizeof( uint16_t );

//        dv.first_hop.copyTo( (uint8_t*)(buffer + offset) );
        uint16_t nfirsthop = htons(dv.first_hop);
        memcpy( f.payload + offset, &nfirsthop, sizeof(uint16_t) );
        offset += sizeof( uint16_t );

        f.payload[ offset++ ] = (uint8_t)(dv.hops);
    }
    UMN_DEBUG_PRINT( "offset = %d\n", offset );
    f.payload_size = offset;
    f.sender_id = _parent->id();
    f.dest_id = 0xffff; // Broadcast
    f.type = UMN_FRAME_REVERSE_ROUTING_TABLE;
    return f;
}
