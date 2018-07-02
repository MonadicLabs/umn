
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
        cerr << "received dvm !" << endl;
        std::vector< DistanceVector > dvm = parseDistanceVectorMessage( f->payload_ptr(), f->getPayloadLength() );
        cerr << "_routes=" << _routes.size() << endl;
        for( int k = 0; k < dvm.size(); ++k )
        {
            printDistanceVector( dvm[k] );
        }
        if( dvm.size() == 0 && _routes.size() == 0 )
        {
            DistanceVector ndv;
            ndv.source = f->getSender();
            ndv.hops = 1;
            ndv.first_hop = _parent->address();
            _routes.push_back(ndv);
            cerr << "WILL ADD ENTRY ! " << endl;
        }
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
    buffer[ offset++ ] = (uint8_t)(_routes.size());

    for( int k = 0; k < _routes.size(); ++k )
    {
        DistanceVector dv = _routes[k];
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
    std::shared_ptr< Frame > f = buildDistanceVectorMessage();
    f->setType( Frame::DISTANCE_VECTOR_MESSAGE );
    f->setSender( _parent->address() );
    _parent->broadcastSend( f );
}
