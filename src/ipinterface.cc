
#include "ipinterface.h"
#include "utils.h"
#include "node.h"

#include <assert.h>

#include <iostream>
using namespace std;

#define ASSERT(popo) assert(popo)

static void alloc_cb_ip(uv_handle_t* handle,
                     size_t suggested_size,
                     uv_buf_t* buf) {
    static char slab[65536];
    // CHECK_HANDLE(handle);
    ASSERT(suggested_size <= sizeof(slab));
    buf->base = slab;
    buf->len = sizeof(slab);
}

static void cl_recv_cb_ip(uv_udp_t* handle,
                       ssize_t nread,
                       const uv_buf_t* buf,
                       const struct sockaddr* addr,
                       unsigned flags) {
    // CHECK_HANDLE(handle);
    // ASSERT(flags == 0);


    // IpServiceDiscovery* ref = (IpServiceDiscovery*)(handle->data);

    printf("nread=%d\n", nread);

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

}

IPInterface::IPInterface(Node *parent, const std::string &ifaceName)
    :CommInterface(parent), _ifaceName(ifaceName)
{
    _serviceDiscovery = new IpServiceDiscovery(this);
    _serviceDiscovery->start();

    initPacketReception();
}

IPInterface::~IPInterface()
{

}

int IPInterface::send(Packet &p)
{
    uint8_t tmpBuffer[1024];
    int sSize = p.copyToBuffer(tmpBuffer, 1024);

    // For every interface listed with the right recepient id, send the packet
    if( p.getRecipientId() == 0 )
    {
        for( std::string ip : _serviceDiscovery->getNeighborIPs() )
        {
            cerr << "################ SENDING PACKET TO IP=" << ip << endl;
            novadem::link::UDPTransmitter udpt( 8001, ip, this->_ifaceName );
            udpt.send( tmpBuffer, sSize );
        }
    }
    else
    {
        std::string ip = _serviceDiscovery->findNodeId( p.getRecipientId() );
        if( ip.size() )
        {
            cerr << "################ SENDING PACKET TO IP=" << ip << endl;
            novadem::link::UDPTransmitter udpt( 8001, ip, this->_ifaceName );
            udpt.send( tmpBuffer, sSize );
        }
    }
}

void IPInterface::initPacketReception()
{
    int r;
    struct sockaddr_in addr;
    assert(0 == uv_ip4_addr( getInterfaceIP(_ifaceName).c_str(), 8001, &addr));
    r = uv_udp_init(uv_default_loop(), &_uvRecv);
    assert(r == 0);
    r = uv_udp_bind(&_uvRecv, (const struct sockaddr*) &addr, UV_UDP_REUSEADDR);
    assert(r == 0);
    _uvRecv.data = (void*)(this);
    r = uv_udp_recv_start(&_uvRecv, alloc_cb_ip, cl_recv_cb_ip);
    assert(r == 0);
}

void on_send(uv_udp_send_t *req, int status) {
    cerr << "####################################### sen !" << endl;
}
