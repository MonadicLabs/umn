#pragma once

#include "frame.h"
#include "stream.h"

#include <cstdio>
#include <vector>

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
    } DistanceVector;

    Router( UMN* parent = nullptr )
        :_parent(parent)
    {

    }

    virtual ~Router()
    {

    }

    virtual void onNewFrame( Stream* s, Frame& f )
    {
        printf( "ROUTER NEW FRAME\n" );
    }

    virtual void tick()
    {
        broadcastRoutingTable();
    }

private:
    void broadcastRoutingTable();
    Frame createRoutingTableFrame();

protected:
    UMN* _parent;
    std::vector< DistanceVector > _reverseRoutes;

};
}
