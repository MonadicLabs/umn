
#include <iostream>

#include <unistd.h>

#include "node.h"
#include "comminterface.h"
#include "ipinterface.h"

#include <uv.h>

using namespace std;
using namespace umn;

uv_loop_t *loop;
uv_udp_t send_socket;
uv_udp_t recv_socket;

static void sv_recv_cb(uv_udp_t* handle,
                       ssize_t nread,
                       uv_buf_t buf,
                       struct sockaddr* addr,
                       unsigned flags) {
    if( nread > 0 )
    cout << "received some shit nread=" << nread << " - " << buf.base << endl;

}

static uv_buf_t alloc_cb(uv_handle_t* handle, size_t suggested_size) {
  static char slab[65536];

  // CHECK_HANDLE(handle);
  // ASSERT(suggested_size <= sizeof slab);

  return uv_buf_init(slab, sizeof slab);
}

int main( int argc, char** argv )
{

//    loop = uv_default_loop();

//    uv_udp_init(loop, &recv_socket);
//    struct sockaddr_in recv_addr;
//    recv_addr = uv_ip4_addr("192.168.1.43", 6800);
//    uv_udp_bind(&recv_socket, recv_addr,0);

//    uv_udp_recv_start(&recv_socket, alloc_cb, sv_recv_cb);


    ShortId a;
    ShortId b(a);

    CommInterface* com1 = new umn::ip::IpInterface(argv[1]);
    Node* n = new Node();

    cout << "Starting NODE " << n->id().toString() << endl;

    n->addCommInterface(com1);
    n->start();

    while(1)
    {
        for( auto kv : n->_neighbors )
        {
            cout << "node " << kv.first.toString() << endl;
        }
        sleep(1);
    }


    //uv_run(loop, UV_RUN_DEFAULT);

    return 0;
}
