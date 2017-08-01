
#include "ipinterface.h"

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

    IPInterface* ref = (IPInterface*)(handle->data);
    ref->processBeaconData((uint8_t*)(buf.base), nread, "10.0.0.1" );
}

void timer_cb1 (uv_timer_t* timer, int status) {
    IPInterface* ref = (IPInterface*)(timer->data);
    ref->decreaseTTL();
}


IPInterface::IPInterface(Node *parent, const std::string &ifaceName)
    :CommInterface(parent), _ifaceName(ifaceName)
{
    initBeaconReception();
}

IPInterface::~IPInterface()
{

}

void on_send(uv_udp_send_t *req, int status) {
  cerr << "####################################### sen !" << endl;
}

void IPInterface::test_send()
{

    cerr << "sedinfdin ??" << endl;
    IpReference ir = _iprefs[ sole::rebuild("00000000-0000-00ff-0100-000000000000") ];
    uv_udp_t poop = ir.send_socket;
    struct sockaddr_in send_addr =  uv_ip4_addr( "10.0.0.1", 10005 );
    uv_udp_send_t * send_req = new uv_udp_send_t;
    uv_buf_t discover_msg = uv_buf_init("PING", 4);
    int r = uv_udp_send(send_req, &poop, &discover_msg, 1, send_addr, on_send);
    cerr << "r_send=" << r << endl;

}

void IPInterface::initBeaconReception()
{
    struct sockaddr_in addr = uv_ip4_addr("10.0.0.27", 12345);
    int r = uv_udp_init(uv_default_loop(), &_beaconClient);
    _beaconClient.data = this;
    r = uv_udp_bind(&_beaconClient, addr, 0);
    r = uv_udp_set_membership(&_beaconClient, "239.255.0.1", NULL, UV_JOIN_GROUP);
    r = uv_udp_recv_start(&_beaconClient, IPINTERFACE_alloc_cb, IPINTERFACE_BEACONCLIENT_recv_cb);

    uv_timer_init( uv_default_loop(), &_ttlTimer );
    _ttlTimer.data = this;
    uv_timer_start(&_ttlTimer, (uv_timer_cb) &timer_cb1, 100, 100);
}

void IPInterface::processBeaconData(uint8_t *buffer, size_t bufferSize, std::string ip )
{
    cerr << "received " << bufferSize << " bytes !" << endl;
    // _parser.appendData( buffer, bufferSize );

    if( bufferSize == 128/8 )
    {
        // Retrrieve sender id
        uint64_t ab, cd;
        memcpy( &ab, buffer, 8 );
        memcpy( &cd, buffer + 8, 8 );

        sole::uuid sid = sole::rebuild( ab, cd );
        cerr << "ppopo received id: " << sid << endl;

        // Check if already known
        if( _iprefs.find( sid ) == _iprefs.end() )
        {
            cerr << "new ref !" << endl;
            IpReference ir;
            ir.ip = ip;
            ir.ttl = 128;
            uv_udp_init( uv_default_loop(), &ir.send_socket );
             struct sockaddr_in addr = uv_ip4_addr( "10.0.0.1", 10005 );
            uv_udp_bind(&ir.send_socket, addr, 0);

            _iprefs.insert( make_pair(sid, ir ) );
        }

        {
            cerr << "updating ref for ip=" << ip << endl;
            _iprefs[ sid ].ttl = 128;
        }
    }

    test_send();

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
