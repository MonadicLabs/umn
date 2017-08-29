
#include "ipinterface.h"
#include "utils.h"
#include "node.h"

#include <iostream>
using namespace std;

static uv_buf_t IPINTERFACE_alloc_cb(uv_handle_t* handle, size_t suggested_size) {
    static char slab[65536];
    return uv_buf_init(slab, sizeof slab);
}

static void IPINTERFACE_BEACONCLIENT_recv_cb(uv_udp_t* handle,
                                             ssize_t nread,
                                             uv_buf_t buf,
                                             struct sockaddr* addr,
                                             unsigned flags)
{
    if (nread < 0) {

    }

    if (nread == 0) {
        return;
    }

    struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
    char *s = inet_ntoa(addr_in->sin_addr);

    IPInterface* ref = (IPInterface*)(handle->data);
    ref->processBeaconData((uint8_t*)(buf.base), nread, s );
}

void timer_cb1 (uv_timer_t* timer, int status) {
    IPInterface* ref = (IPInterface*)(timer->data);
    ref->decreaseTTL();
}

void xmit_timer_cb (uv_timer_t* timer, int status) {
    // cerr << "xmit !" << endl;
    IPInterface* ref = (IPInterface*)(timer->data);
    ref->emitBeacon();
}

IPInterface::IPInterface(Node *parent, const std::string &ifaceName)
    :CommInterface(parent), _ifaceName(ifaceName)
{
    initBeaconReception();
    initBeaconEmission();
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

void IPInterface::initBeaconReception()
{
    struct sockaddr_in addr = uv_ip4_addr( "0.0.0.0", 12345);
    int r = uv_udp_init(uv_default_loop(), &_beaconClient);
    _beaconClient.data = this;
    r = uv_udp_bind(&_beaconClient, addr, 0);

    r = uv_udp_set_membership(&_beaconClient, "239.255.0.1", novadem::link::getNetworkInterfaceIP(_ifaceName).c_str(), UV_JOIN_GROUP);
    r = uv_udp_set_multicast_loop(&_beaconClient, 0);
    r = uv_udp_recv_start(&_beaconClient, IPINTERFACE_alloc_cb, IPINTERFACE_BEACONCLIENT_recv_cb);

    uv_timer_init( uv_default_loop(), &_ttlTimer );
    _ttlTimer.data = this;
    uv_timer_start(&_ttlTimer, (uv_timer_cb) &timer_cb1, 100, 100);
}

void IPInterface::initBeaconEmission()
{
    _beaconTransmitter = new novadem::link::UDPTransmitter( 12345, "239.255.0.1", _ifaceName );

    // Create a libuv timer that sends a beacon every xth of a second
    uv_timer_init( uv_default_loop(), &_xmitTimer );
    _xmitTimer.data = this;
    uv_timer_start( &_xmitTimer, (uv_timer_cb) &xmit_timer_cb, 100, 100 );
}

void IPInterface::processBeaconData(uint8_t *buffer, size_t bufferSize, std::string ip )
{
    // cerr << "received " << bufferSize << " bytes ! from " << ip << endl;

    if( bufferSize == 128/8 )
    {
        // Retrrieve sender id
        uint64_t ab, cd;
        memcpy( &ab, buffer, 8 );
        memcpy( &cd, buffer + 8, 8 );

        sole::uuid sid = sole::rebuild( ab, cd );

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

void IPInterface::emitBeacon()
{
    char popo[1024];
    sole::uuid u = _parent->getId();
    memcpy( popo, &(u.ab), 64 / 8 );
    memcpy( popo + 8, &(u.cd), 64 / 8 );
    _beaconTransmitter->send( popo, 128 / 8 );
}
