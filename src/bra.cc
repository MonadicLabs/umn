
#include "frame.h"
#include "bra.h"
#include "node.h"

void umn::BRA::printDistanceVector(umn::BRA::DistanceVector dv)
{
    cerr << "[ src:" << dv.source.asInteger() << " hops:" << dv.hops << " next:" << dv.first_hop.asInteger() << " ]" << endl;
}

umn::BRA::BRA(umn::Node *parent)
    :Router(parent)
{
    _broadcastTimer.start();
}

umn::BRA::~BRA()
{

}

void umn::BRA::tick()
{
    if( _broadcastTimer.getElapsedTimeInMilliSec() >= BRA_BROADCAST_INTERVAL_MS )
    {
        broadcastRoutingTable();
        _broadcastTimer.start();
    }
}

void umn::BRA::processFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<umn::Transport> t)
{
    switch( f->getType() )
    {
    case Frame::DISTANCE_VECTOR_MESSAGE:
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

        std::vector< DistanceVector > dvm = parseDistanceVectorMessage( f->payload_ptr(), f->getPayloadLength() );
        cerr << "received dvm ! n=" << dvm.size() << endl;
        // Now print routes
        cerr << "---- RECVD ROUTES from " << f->getSender().asInteger() << " ----" << endl;
        for( DistanceVector rdv : dvm )
            printDistanceVector(rdv);
        cerr << "-----------------------------" << endl;

        // Check if sender node is already known from us...
        bool alreadyReceived = false;
        for( DistanceVector re : _reverseRoutes )
        {
            if( re.source == f->getSender() )
            {
                alreadyReceived = true;
                break;
            }
        }
        if( !alreadyReceived )
        {
            DistanceVector ndv;
            ndv.source = f->getSender();
            ndv.first_hop = _parent->address();
            ndv.hops = 1;
            _reverseRoutes.push_back( ndv );
        }

        // Now, parse the distance vector message...
        for( DistanceVector dv : dvm )
        {
            // Have we received the reverse route from OURSELF to f->getSender() .. ?
            if( dv.source == _parent->address() )
            {
                // Alright, check if already in there, but we found a REVERSE ROUTE
                // from ourself to the sender !
                if( _routingTable.find( f->getSender() ) == _routingTable.end() )
                {
                    RouteEntry re;
                    re.hops = dv.hops;
                    re.next = dv.first_hop;
                    _routingTable.insert( make_pair( f->getSender(), re ) );
                }
            }
        }

        for( DistanceVector dv : dvm )
        {
            // Check if we have already a reverse route from all the sources of this dvm
            bool routeFound = false;
            for( DistanceVector& re : _reverseRoutes )
            {
                if( re.source == dv.source )
                {
                    if( re.hops > dv.hops + 1 )
                    {
                        re = dv;
                        re.hops++;
                    }
                    routeFound = true;
                }
            }
            if( !routeFound )
            {
                DistanceVector ndv = dv;
                ndv.hops++;
                _reverseRoutes.push_back(ndv);
            }
        }

        // Now print routes
        cerr << "---- KNOWN ROUTES ----" << endl;
        for( DistanceVector rdv : _reverseRoutes )
            printDistanceVector(rdv);
        cerr << "----------------------" << endl;

        cerr << "---- ROUTING TABLE ----" << endl;
        printRoutingTable();
        cerr << "-----------------------" << endl;

        break;
    }
    default:
        break;
    }
}

std::shared_ptr<umn::Frame> umn::BRA::buildDistanceVectorMessage()
{
    std::shared_ptr< Frame > f = std::make_shared<Frame>();

    int offset = 0;
    unsigned char buffer[ 1024 ];

    // Write number of entries
    buffer[ offset++ ] = (uint8_t)(_reverseRoutes.size());

    for( int k = 0; k < _reverseRoutes.size(); ++k )
    {
        DistanceVector dv = _reverseRoutes[k];
        dv.source.copyTo( (uint8_t*)(buffer + offset) );
        offset += sizeof( uint32_t );
        dv.first_hop.copyTo( (uint8_t*)(buffer + offset) );
        offset += sizeof( uint32_t );
        buffer[ offset++ ] = (uint8_t)(dv.hops);
    }

    f->setPayload( buffer, offset );
    f->setSender( _parent->address() );
    return f;
}

std::vector< umn::BRA::DistanceVector > umn::BRA::parseDistanceVectorMessage(unsigned char *buffer, size_t len)
{
    int offset = 0;
    uint8_t nentries = buffer[ offset++ ];
    cerr << "parse_nentries=" << (int)nentries << endl;
    std::vector< DistanceVector > entries;
    for( int k = 0; k < (int)nentries; ++k )
    {
        DistanceVector entry;
        entry.source = NodeAddress::fromMemory( buffer + offset, 4 );
        offset += sizeof(uint32_t);
        entry.first_hop = NodeAddress::fromMemory( buffer + offset, 4 );
        offset += sizeof(uint32_t);
        entry.hops = buffer[ offset++ ];
        entries.push_back(entry);
    }
    return entries;
}

void umn::BRA::broadcastRoutingTable()
{
    // std::shared_ptr< Frame > f = buildDistanceVectorMessage();
    std::shared_ptr< Frame > f = buildDistanceVectorMessage();
    f->setType( Frame::DISTANCE_VECTOR_MESSAGE );
    f->setSender( _parent->address() );
    _parent->broadcastSend( f );
}

void umn::BRA::printRoutingTable()
{
    for( auto kv : _routingTable )
    {
        cerr << "TO: " << kv.first.asInteger() << " - ROUTE: " << kv.second.next.asInteger() << " - HOPS: " << kv.second.hops << endl;
    }
}
