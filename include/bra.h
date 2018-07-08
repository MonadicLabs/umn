#pragma once

#include <vector>
#include <map>

#include "router.h"
#include "timer.h"

namespace umn
{
    class BRA : public Router
    {
    public:

        typedef struct
        {
            NodeAddress source;
            NodeAddress first_hop;
            int hops;
            Timer validityTimer;
        } DistanceVector;

        typedef struct
        {

            std::shared_ptr< Transport > transport;

        } MultiPathInformation;

        typedef struct
        {
            NodeAddress next;
            int hops;
            Timer validityTimer;
        } RouteEntry;

        // DEBUG
        void printDistanceVector( DistanceVector dv );
        //

        static const int BRA_BROADCAST_INTERVAL_MS = 500;
        static const int BRA_ROUTE_VALIDITY_TIMEOUT = 2000;

        BRA( Node* parent );
        virtual ~BRA();

        virtual void tick();

        virtual void processFrame( std::shared_ptr<Frame> f, std::shared_ptr< Transport > t );

    private:
        Timer _broadcastTimer;
        Timer _invalidationTimer;

        std::shared_ptr< Frame > buildDistanceVectorMessage();
        std::vector<DistanceVector> parseDistanceVectorMessage( unsigned char * buffer, size_t len );
        void broadcastRoutingTable();
        void invalidateRoutes();

        std::vector< DistanceVector > _reverseRoutes;
        std::map< NodeAddress, RouteEntry > _routingTable;
        std::map< NodeAddress, std::vector< MultiPathInformation > > _paths;

        void printRoutingTable();

    };
}
