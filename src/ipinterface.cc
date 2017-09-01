
#include "ipinterface.h"
#include "utils.h"
#include "node.h"

#include <iostream>
using namespace std;

IPInterface::IPInterface(Node *parent, const std::string &ifaceName)
    :CommInterface(parent), _ifaceName(ifaceName)
{
    _serviceDiscovery = new IpServiceDiscovery(this);
    _serviceDiscovery->start();
}

IPInterface::~IPInterface()
{

}

int IPInterface::send(Packet &p)
{
    uint8_t tmpBuffer[1024];
    int sSize = p.copyToBuffer(tmpBuffer, 1024);

    // For all listed IP addresses, send a copy of packet
    for( auto kv : this->_iprefs )
    {
        cerr << "################ SENDING PACKET TO IP=" << kv.second.ip << endl;
        novadem::link::UDPTransmitter udpt( 5000, kv.second.ip, this->_ifaceName );
        udpt.send( tmpBuffer, sSize );
    }
}

void on_send(uv_udp_send_t *req, int status) {
    cerr << "####################################### sen !" << endl;
}

void IPInterface::test_send()
{

}

void IPInterface::processBeaconData(uint8_t *buffer, size_t bufferSize, std::string ip )
{
    // cerr << "received " << bufferSize << " bytes ! from " << ip << endl;

    if( bufferSize == 128/8 )
    {
        // Retrrieve sender id
        NodeIdType sid;
        memcpy( &sid, buffer, sizeof(NodeIdType) );

        // Check if it's our own beacon
        if( ip == novadem::link::getNetworkInterfaceIP(_ifaceName) )
        {
            // cerr << "own beacon." << endl;
            return;
        }

        cerr << "ppopo received id: " << sid << endl;

        // Check if already known
        if( _iprefs.find( sid ) == _iprefs.end() )
        {
            cerr << "new ref !" << endl;
            IpReference ir;
            ir.ip = ip;
            ir.ttl = 128;
            _iprefs.insert( make_pair(sid, ir ) );
        }

        {
            cerr << "updating ref for ip=" << ip << endl;
            _iprefs[ sid ].ttl = 128;
        }
    }
    // test_send();
}

void IPInterface::decreaseTTL()
{
    for( auto& kv : _iprefs )
    {
        kv.second.ttl -= 1;
    }

    for (auto it = _iprefs.cbegin(); it != _iprefs.cend() /* not hoisted */; /* no increment */)
    {
        if ( it->second.ttl <= 0 )
        {
            cerr << "erasing ip=" << it->second.ip << endl;
            _iprefs.erase(it++);    // or "it = m.erase(it)" since C++11
        }
        else
        {
            ++it;
        }
    }

}
