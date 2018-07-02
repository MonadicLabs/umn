#pragma once

#include <vector>

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
            std::shared_ptr< Transport > transport;
        } DistanceVector;

        // DEBUG
        void printDistanceVector( DistanceVector dv );
        //

        static const int BRA_BROADCAST_INTERVAL_MS = 500;

        BRA( Node* parent );
        virtual ~BRA();

        virtual void tick();

        virtual void processFrame( std::shared_ptr<Frame> f, std::shared_ptr< Transport > t );

    private:
        Timer _broadcastTimer;

        std::shared_ptr< Frame > buildDistanceVectorMessage();
        std::vector<DistanceVector> parseDistanceVectorMessage( unsigned char * buffer, size_t len );
        void broadcastRoutingTable();

        std::vector< DistanceVector > _routes;

    };
}
