
#include "ipservicediscovery.h"
#include "ipinterface.h"
#include "node.h"
#include "utils.h"

#include <assert.h>

#define ASSERT(handle) assert(handle)

void xmit_timer_cb (uv_timer_t* timer, int status) {
    // cerr << "xmit !" << endl;
    IpServiceDiscovery* ref = (IpServiceDiscovery*)(timer->data);
    ref->emitBeacon();
}

static void alloc_cb(uv_handle_t* handle,
                     size_t suggested_size,
                     uv_buf_t* buf) {
    static char slab[65536];
    // CHECK_HANDLE(handle);
    ASSERT(suggested_size <= sizeof(slab));
    buf->base = slab;
    buf->len = sizeof(slab);
}

static void cl_recv_cb(uv_udp_t* handle,
                       ssize_t nread,
                       const uv_buf_t* buf,
                       const struct sockaddr* addr,
                       unsigned flags) {
    // CHECK_HANDLE(handle);
    // ASSERT(flags == 0);


    IpServiceDiscovery* ref = (IpServiceDiscovery*)(handle->data);

    // printf("nread=%d\n", nread);

    if (nread < 0) {
        ASSERT(0 && "unexpected error");
    }

    if (nread == 0) {
        /* Returning unused buffer */
        /* Don't count towards cl_recv_cb_called */
        ASSERT(addr == NULL);
        return;
    }

    ASSERT(addr != NULL);
    // ASSERT(nread == 4);
    // ASSERT(!memcmp("PING", buf->base, nread));
    struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
    char *s = inet_ntoa(addr_in->sin_addr);
    // cerr << "s=" << s << endl;

    /* we are done with the client handle, we can close it */
    // uv_close((uv_handle_t*) &client, close_cb);
    ref->processBeaconData( buf->base, nread, s );

}


static void IPINTERFACE_BEACONCLIENT_recv_cb(uv_udp_t* handle,
                                             ssize_t nread,
                                             uv_buf_t buf,
                                             struct sockaddr* addr,
                                             unsigned flags)
{

    // printf("nread=%d\n", nread);

    if (nread < 0) {
        ASSERT(0 && "unexpected error");
    }

    if (nread == 0) {
        /* Returning unused buffer */
        /* Don't count towards cl_recv_cb_called */
        ASSERT(addr == NULL);
        return;
    }

    ASSERT(addr != NULL);
    // ASSERT(nread == 4);
    // ASSERT(!memcmp("PING", buf->base, nread));
    struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
    char *s = inet_ntoa(addr_in->sin_addr);
    // cerr << "s=" << s << endl;

    /* we are done with the client handle, we can close it */
    // uv_close((uv_handle_t*) &client, close_cb);


}

void timer_cb1 (uv_timer_t* timer, int status)
{
    IpServiceDiscovery* ref = (IpServiceDiscovery*)(timer->data);
    ref->decreaseTTL();
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
    _beaconTransmitter->send( popo, sizeof(NodeIdType) );
}

void IpServiceDiscovery::processBeaconData(uint8_t *buffer, size_t bufferSize, string ip)
{

    // cerr << "## received " << bufferSize << " bytes ! from " << ip << endl;

    if( bufferSize == sizeof(NodeIdType) )
    {
        // Retrrieve sender id
        NodeIdType sid;
        memcpy( &sid, buffer, sizeof(NodeIdType) );

        // Check if it's our own beacon
        std::string ifname = _parent->getInterfaceName();
        std::string iip = getInterfaceIP( ifname );
        if( ip == iip )
        {
            // cerr << "own beacon." << endl;
            return;
        }

        // cerr << "ppopo received id: " << sid << endl;

        // Check if already known
        if( _ipRefs.find( sid ) == _ipRefs.end() )
        {
            cerr << "new ref !" << endl;
            IpTimeRef ir;
            ir.ip = ip;
            ir.ttl = 128;
            _ipRefs.insert( make_pair( sid, ir ) );
        }

        {
            // cerr << "updating ref for ip=" << ip << endl;
            _ipRefs[ sid ].ttl = 128;
        }

    }
    // test_send();
}

void IpServiceDiscovery::decreaseTTL()
{
    for( auto& kv : _ipRefs )
    {

        kv.second.ttl -= 10;
    }

    for (auto it = _ipRefs.cbegin(); it != _ipRefs.cend(); )
    {
        // cerr << "TTL for " << it->second.ip << " is " << it->second.ttl << endl;
        if ( it->second.ttl <= 0 )
        {
            cerr << "erasing ip=" << it->second.ip << endl;
            _ipRefs.erase(it++);    // or "it = m.erase(it)" since C++11
        }
        else
        {
            ++it;
        }
    }
}

string IpServiceDiscovery::findNodeId(NodeIdType id)
{
    if( _ipRefs.find(id) != _ipRefs.end() )
        return "";
    else
        return _ipRefs[ id ].ip;
}

std::vector<string> IpServiceDiscovery::getNeighborIPs()
{
    std::vector< std::string > ips;
    for( auto kv : _ipRefs )
    {
        ips.push_back( kv.second.ip );
    }
    return ips;
}

void IpServiceDiscovery::initBeaconReception()
{
    int r;
    uv_udp_send_t req;
    uv_buf_t buf;
    struct sockaddr_in addr;

    assert(0 == uv_ip4_addr("0.0.0.0", 49800, &addr));
    r = uv_udp_init(uv_default_loop(), &this->_beaconClient);
    assert(r == 0);

    /* bind to the desired port */
    r = uv_udp_bind(&_beaconClient, (const struct sockaddr*) &addr, UV_UDP_REUSEADDR);
    assert(r == 0);

    r = uv_udp_set_membership(&_beaconClient, "225.0.0.37", getInterfaceIP( "eth0" ).c_str(), UV_JOIN_GROUP);
    if (r == UV_ENODEV)
        printf("No multicast support.\n");
    assert(r == 0);

    _beaconClient.data = (void*)(this);

    r = uv_udp_recv_start(&_beaconClient, alloc_cb, cl_recv_cb);
    assert(r == 0);

    uv_timer_init( uv_default_loop(), &_ttlTimer );
    _ttlTimer.data = this;
    uv_timer_start(&_ttlTimer, (uv_timer_cb) &timer_cb1, 100, 100);

}

void IpServiceDiscovery::initBeaconEmission()
{
    _beaconTransmitter = new novadem::link::UDPTransmitter( 49800, "225.0.0.37", _parent->getInterfaceName() );

    // Create a libuv timer that sends a beacon every xth of a second
    uv_timer_init( uv_default_loop(), &_xmitTimer );
    _xmitTimer.data = this;
    uv_timer_start( &_xmitTimer, (uv_timer_cb) &xmit_timer_cb, 100, 100 );
}

