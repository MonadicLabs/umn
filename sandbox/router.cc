
#define DEBUG 1

#include "router.h"
#include "umn.h"
#include "frame.h"

#include <cstring>

void umn::Router::onNewFrame(umn::Stream *s, umn::Frame &f)
{
    printf( "ROUTER NEW FRAME\n" );
    switch( f.type )
    {
    case UMN_FRAME_REVERSE_ROUTING_TABLE:
    {
        /*
                a node "B" receiving a distance vector from one of its
                in-neighbors "A" extracts the following information:
                    -   The reverse route from "B" to "A"; B obtains this informa-
                        tion from the entry for the route from B to A if the dis-
                        tance vector includes such an entry.
                    -   If the currently known route from node C to B ( C ~~>i B )
                        is longer than the route from C to A followed by A -> B
                        (i.e., C ~~>j A and i > j + 1), then B sets
                        newly found shorter route from C to B through A.
                */
        std::vector< ReverseDistanceVector > dvm = parseReverseRoutingEntries( f.payload, f.payload_size );
        UMN_DEBUG_PRINT( "received dvm ! n=%d\n", dvm.size() );
        // Now print routes
        UMN_DEBUG_PRINT( "---- RECVD ROUTES from %d ----\n", f.sender_id );
        for( ReverseDistanceVector rdv : dvm )
        {
            printDistanceVector(rdv);
        }
        UMN_DEBUG_PRINT( "-----------------------------\n" );

        // Check if sender node is already known from us...
        bool alreadyReceived = false;
        for( ReverseDistanceVector& re : _reverseRoutes )
        {
            if( re.source == f.sender_id )
            {
                // re.validityTimer.start();
                alreadyReceived = true;
                break;
            }
        }
        if( !alreadyReceived )
        {
            ReverseDistanceVector ndv;
            ndv.source = f.sender_id;
            ndv.first_hop = _parent->id();
            ndv.hops = 1;
            // ndv.validityTimer.start();
            _reverseRoutes.push_back( ndv );
        }

        // Now, parse the distance vector message...
        for( ReverseDistanceVector dv : dvm )
        {
            // Have we received the reverse route from OURSELF to f->sender_id .. ?
            if( dv.source == _parent->id() )
            {
                // Alright, check if already in there, but we found a REVERSE ROUTE
                // from ourself to the sender !
                if( _routingTable.find( f.sender_id ) == _routingTable.end() )
                {
                    RouteEntry re;
                    re.hops = dv.hops;
                    re.next = dv.first_hop;
                    //                    re.validityTimer.start();
                    _routingTable.insert( make_pair( f.sender_id, re ) );
                }
            }
        }

        for( ReverseDistanceVector dv : dvm )
        {
            // Check if we have already a reverse route from all the sources of this dvm
            bool routeFound = false;
            for( ReverseDistanceVector& re : _reverseRoutes )
            {
                if( re.source == dv.source )
                {
                    if( re.hops > dv.hops + 1 )
                    {
                        re = dv;
                        re.hops++;
                        //                        re.validityTimer.start();
                    }
                    routeFound = true;
                }
            }
            if( !routeFound )
            {
                ReverseDistanceVector ndv = dv;
                ndv.hops++;
                //                ndv.validityTimer.start();
                _reverseRoutes.push_back(ndv);
            }
        }

        // Now print routes
        UMN_DEBUG_PRINT( "---- KNOWN ROUTES ----\n" );
        for( ReverseDistanceVector rdv : _reverseRoutes )
        {
            printDistanceVector(rdv);
        }
        UMN_DEBUG_PRINT( "----------------------\n" );

        UMN_DEBUG_PRINT( "---- ROUTING TABLE ----\n" );
        printRoutingTable();
        UMN_DEBUG_PRINT( "-----------------------\n" );

        break;
    }

    default:
        break;

    }
}

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
        ReverseDistanceVector dv = _reverseRoutes[k];
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

void umn::Router::printDistanceVector(ReverseDistanceVector &dv)
{
    // cerr << "[ src:" << dv.source.asInteger() << " hops:" << dv.hops << " next:" << dv.first_hop.asInteger() << " ]" << endl;
    UMN_DEBUG_PRINT( "[ src:%d hops:%d next: %d\n", dv.source, dv.hops, dv.first_hop );
}

void umn::Router::printRoutingTable()
{
    for( auto kv : _routingTable )
    {
        UMN_DEBUG_PRINT( "TO: %d - ROUTE: %d - HOPS: %d", kv.first, kv.second.next, kv.second.hops );
    }
}

std::vector<umn::Router::ReverseDistanceVector> umn::Router::parseReverseRoutingEntries(uint8_t *buffer, size_t buffer_size)
{
    int offset = 0;
    uint8_t nentries = buffer[ offset++ ];
    UMN_DEBUG_PRINT( "parse_nentries=%d\n" , (int)nentries );
    std::vector< ReverseDistanceVector > entries;
    for( int k = 0; k < (int)nentries; ++k )
    {
        ReverseDistanceVector entry;

        uint16_t hsource;
        memcpy( &hsource, buffer + offset, sizeof(uint16_t) );
        offset += sizeof( uint16_t );
        hsource = ntohs(hsource);
        entry.source = hsource;

        uint16_t hfhops;
        memcpy( &hfhops, buffer + offset, sizeof(uint16_t) );
        offset += sizeof( uint16_t );
        hfhops = ntohs(hfhops);
        entry.first_hop = hfhops;

        entry.hops = buffer[ offset++ ];

        entries.push_back(entry);
    }
    return entries;
}
