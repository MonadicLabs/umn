
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
        int ttl = f.payload[ 0 ];

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
                if( _reverseRoutingTable.find( f.sender_id ) == _reverseRoutingTable.end() )
                {
                    RouteEntry re;
                    re.hops = dv.hops;
                    re.next = dv.first_hop;
                    //                    re.validityTimer.start();
                    _reverseRoutingTable.insert( make_pair( f.sender_id, re ) );
                }
                else
                {
                    // printf( "ALREADY HAVE ROUTING ENTRY: \n" );
                    // printRoutingTable();
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
                    if( re.hops > dv.hops )
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

        UMN_DEBUG_PRINT( "---- REVERSE ROUTING TABLE ----\n" );
        printReverseRoutingTable();
        UMN_DEBUG_PRINT( "-----------------------\n" );

        if( ttl - 1 > 0 )
        {
            forwardRoutingTableFrame( f );
        }

        break;
    }

    case UMN_FRAME_HELLO:
    {
        UMN_DEBUG_PRINT( "##### RECEIVED HELLO FROM: %d\n", f.sender_id );
        uint8_t curTTL = f.payload[ 0 ];
        if( curTTL <= 1 )
        {
            // Nope
        }
        else
        {
            forwardHello( f );
        }
    }

    default:
        break;

    }
}

void umn::Router::route(umn::Frame &f)
{
    UMN_DEBUG_PRINT( "Routing from %d to %d...\n", f.sender_id, f.dest_id );
    // Find best route for destination...

    // And forward to the right node, using the right interface
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

umn::Frame umn::Router::createRoutingTableFrame(int ttl)
{
    Frame f;
    int offset = 0;

    // Write TTL
    f.payload[ offset++ ] = ttl;

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

void umn::Router::forwardRoutingTableFrame(umn::Frame &f)
{
    f.payload[ 0 ] -= 1;
    _parent->broadcast( f );
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
        UMN_DEBUG_PRINT( "* TO: %d - ROUTE: %d - HOPS: %d \n", kv.first, kv.second.next, kv.second.hops );
    }
}

void umn::Router::printReverseRoutingTable()
{
    for( auto kv : _reverseRoutingTable )
    {
        UMN_DEBUG_PRINT( "* TO: %d - ROUTE: %d - HOPS: %d \n", kv.first, kv.second.next, kv.second.hops );
    }
}

void umn::Router::broadcastHello(int ttl)
{
    uint8_t buffer[ MAX_FRAME_BUFFER_SIZE ];
    for( int i = 0; i < UMN_MAX_STREAM_NUMBER; ++i )
    {
        Stream * s = _parent->getStream(i);
        if( s )
        {
            Frame hello = createHelloPacket( s, ttl );
            int fsize = frameToBuffer( hello, buffer, MAX_FRAME_BUFFER_SIZE );
            s->write( buffer, fsize );
        }
    }
}

void umn::Router::forwardHello(umn::Frame &f)
{
    // Decrease TTL
    f.payload[ 0 ] = f.payload[ 0 ] - 1;

    //
    uint8_t buffer[ MAX_FRAME_BUFFER_SIZE ];
    for( int i = 0; i < UMN_MAX_STREAM_NUMBER; ++i )
    {
        Stream * s = _parent->getStream(i);
        if( s )
        {
            int fsize = frameToBuffer( f, buffer, MAX_FRAME_BUFFER_SIZE );
            s->write( buffer, fsize );
        }
    }
}

umn::Frame umn::Router::createHelloPacket(umn::Stream *s, int ttl)
{
    Frame ret;
    ret.type = UMN_FRAME_HELLO;
    int offset = 0;
    ret.payload[ offset++ ] = ttl;

    uint16_t n_sid = htons( getStreamId(s) );
    memcpy( ret.payload + offset, &n_sid, sizeof(uint16_t) );
    offset += sizeof( uint16_t );
    ret.payload_size = offset;

    ret.sender_id = _parent->id();
    ret.dest_id = 0xffff;

    return ret;
}

umn::Frame umn::Router::createOHaiPacket(umn::Frame &hello)
{
    Frame ret;
    ret.type = UMN_FRAME_OHAI;
    memcpy( ret.payload, hello.payload, hello.payload_size );
    ret.payload_size = hello.payload_size;
    ret.sender_id = _parent->id();
    ret.dest_id = hello.sender_id;
    return ret;
}

uint32_t umn::Router::hashPointer(void *ptr)
{
    /*
    Over the years, I’ve been using Thomas Wang’s hash function to hash pointers on several platforms and it worked very well in all cases:
    */
    uint32_t Value = (uint32_t)ptr;
    Value = ~Value + (Value << 15);
    Value = Value ^ (Value >> 12);
    Value = Value + (Value << 2);
    Value = Value ^ (Value >> 4);
    Value = Value * 2057;
    Value = Value ^ (Value >> 16);
    return Value;
}

uint16_t umn::Router::hash32to16(uint32_t v)
{
    return v % 0xffff;
}

uint16_t umn::Router::getStreamId(umn::Stream *s)
{
    return hash32to16( hashPointer( s ) );
}

std::vector<umn::Router::ReverseDistanceVector> umn::Router::parseReverseRoutingEntries(uint8_t *buffer, size_t buffer_size)
{
    int offset = 0;

    // Skip TTL
    offset++;

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
