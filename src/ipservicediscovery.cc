
#include "ipservicediscovery.h"
#include "ipinterface.h"
#include "node.h"
#include "utils.h"

#include <assert.h>

void xmit_timer_cb (uv_timer_t* timer, int status) {
    // cerr << "xmit !" << endl;
    IpServiceDiscovery* ref = (IpServiceDiscovery*)(timer->data);
    ref->emitBeacon();
}

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
    cerr << "received " << nread << " bytes " << endl;
    if (nread < 0) {
        cerr << "error." << endl;
        return;
    }

    if (nread == 0) {
        return;
    }

    struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
    char *s = inet_ntoa(addr_in->sin_addr);

    /*
    IPInterface* ref = (IPInterface*)(handle->data);
    ref->processBeaconData((uint8_t*)(buf.base), nread, s );
    */

}

void timer_cb1 (uv_timer_t* timer, int status)
{
    /*
    IPInterface* ref = (IPInterface*)(timer->data);
    ref->decreaseTTL();
    */
}

IpServiceDiscovery::IpServiceDiscovery(IPInterface *parent)
    :_parent(parent)
{

}

IpServiceDiscovery::~IpServiceDiscovery()
{

}

void IpServiceDiscovery::start()
{
    initBeaconEmission();
    initBeaconReception();
}

void IpServiceDiscovery::stop()
{

}

void IpServiceDiscovery::emitBeacon()
{
    // cerr << "emiteBeacon" << endl;
    uint8_t popo[1024];
    NodeIdType u = 1234567; // _parent->getId();
    memcpy( popo, &(u), sizeof(NodeIdType) );
    _beaconTransmitter->send( popo, 512 );
}

void IpServiceDiscovery::initBeaconReception()
{
    struct sockaddr_in addr;
    std::string ipstr = getInterfaceIP( _parent->getInterfaceName() );
    cerr << "ipstr=" << ipstr << endl;
    int r = uv_ip4_addr( ipstr.c_str(), 8000, &addr );
    r = uv_udp_init(uv_default_loop(), &_beaconClient);
    _beaconClient.data = this;
    assert( r == 0 );

    r = uv_udp_bind(&_beaconClient, (const struct sockaddr *)(&addr), 0 );
    assert( r == 0 );

    r = uv_udp_set_membership(&_beaconClient, "224.0.0.7", NULL, UV_JOIN_GROUP);
    if (r == UV_ENODEV)
        cerr << "No multicast support." << endl;
    // r = uv_udp_set_multicast_loop(&_beaconClient, 0);
    r = uv_udp_recv_start(&_beaconClient, IPINTERFACE_alloc_cb, IPINTERFACE_BEACONCLIENT_recv_cb);

    uv_timer_init( uv_default_loop(), &_ttlTimer );
    _ttlTimer.data = this;
    uv_timer_start(&_ttlTimer, (uv_timer_cb) &timer_cb1, 100, 100);
}

void IpServiceDiscovery::initBeaconEmission()
{
    _beaconTransmitter = new novadem::link::UDPTransmitter( 8000, "224.0.0.7", _parent->getInterfaceName() );

    // Create a libuv timer that sends a beacon every xth of a second
    uv_timer_init( uv_default_loop(), &_xmitTimer );
    _xmitTimer.data = this;
    uv_timer_start( &_xmitTimer, (uv_timer_cb) &xmit_timer_cb, 100, 100 );
}
