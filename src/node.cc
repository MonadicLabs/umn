
#include "node.h"


Node::Node()
{
    _nodeId = sole::uuid4();
}

Node::~Node()
{

}

sole::uuid Node::getId()
{
    return _nodeId;
}

void Node::addCommInterface(std::shared_ptr<CommInterface> ci)
{
    _interfaces.push_back( ci );
}

void Node::broadcastBeacon()
{
    // Cook a beacon packet.
    Packet beaconPacket( Packet::UMN_BEACON_PKT );
    beaconPacket.setSenderId( this->getId() );
    beaconPacket.setRecepientId( sole::rebuild(0,0) );

    // Send through every comm interface
    for( std::shared_ptr<CommInterface> ci : _interfaces )
    {
        ci->send( beaconPacket );
    }
}
