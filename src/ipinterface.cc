
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
    ref->processBeaconData((uint8_t*)(buf.base), nread);
}

IPInterface::IPInterface(Node *parent, const std::string &ifaceName)
    :CommInterface(parent), _ifaceName(ifaceName)
{
    initBeaconReception();
}

IPInterface::~IPInterface()
{

}

void IPInterface::initBeaconReception()
{
    struct sockaddr_in addr = uv_ip4_addr("10.0.0.35", 12345);
    int r = uv_udp_init(uv_default_loop(), &_beaconClient);
    _beaconClient.data = this;
    r = uv_udp_bind(&_beaconClient, addr, 0);
    r = uv_udp_set_membership(&_beaconClient, "239.255.0.1", NULL, UV_JOIN_GROUP);
    r = uv_udp_recv_start(&_beaconClient, IPINTERFACE_alloc_cb, IPINTERFACE_BEACONCLIENT_recv_cb);
}

void IPInterface::printBuffer(uv_buf_t buf)
{
    cerr << "received " << buf.len << " bytes !" << endl;
    cerr << buf.base << endl;
}

void IPInterface::processBeaconData(uint8_t *buffer, size_t bufferSize)
{
    cerr << "received " << bufferSize << " bytes !" << endl;
    // cerr << buf.base << endl;
}
