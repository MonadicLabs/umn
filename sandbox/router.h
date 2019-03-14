#pragma once

#include "frame.h"
#include "stream.h"

#include <cstdio>
#include <vector>
#include <map>

namespace umn
{
class UMN;
class Router
{
public:

    typedef struct
    {
        uint16_t source;
        uint16_t first_hop;
        int hops;
        // Timer validityTimer;
    } ReverseDistanceVector;

    typedef struct
    {
        uint16_t next;
        int hops;
//        Timer validityTimer;
    } RouteEntry;

    Router( UMN* parent = nullptr )
        :_parent(parent)
    {

    }

    virtual ~Router()
    {

    }

    virtual void onNewFrame( Stream* s, Frame& f );

    virtual void tick()
    {
        broadcastRoutingTable();
        broadcastHello(5);
    }

    virtual void route( Frame& f );

private:
    void broadcastRoutingTable();
    Frame createRoutingTableFrame();
    void printDistanceVector( ReverseDistanceVector& dv );
    void printRoutingTable();

    void broadcastHello( int ttl );
    void forwardHello( Frame& f );
    Frame createHelloPacket( Stream* s, int ttl );
    Frame createOHaiPacket( Frame& hello );
    uint32_t hashPointer( void* ptr );
    uint16_t hash32to16( uint32_t v );
    uint16_t getStreamId( Stream* s );

protected:
    UMN* _parent;
    std::vector< ReverseDistanceVector > _reverseRoutes;
    std::vector< ReverseDistanceVector > parseReverseRoutingEntries( uint8_t* buffer, size_t buffer_size );
    std::map< uint16_t, RouteEntry > _routingTable;

};
}
