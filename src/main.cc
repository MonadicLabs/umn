/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "uv.h"
#include "udptransmitter.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include <iostream>
using namespace std;

#define ASSERT(handle) assert(handle)

#define CHECK_HANDLE(handle) \
    ASSERT((uv_udp_t*)(handle) == &server || (uv_udp_t*)(handle) == &client)

static uv_udp_t server;
static uv_udp_t client;

static int cl_recv_cb_called;

static int sv_send_cb_called;

static int close_cb_called;

static void alloc_cb(uv_handle_t* handle,
                     size_t suggested_size,
                     uv_buf_t* buf) {
    static char slab[65536];
    CHECK_HANDLE(handle);
    ASSERT(suggested_size <= sizeof(slab));
    buf->base = slab;
    buf->len = sizeof(slab);
}


static void close_cb(uv_handle_t* handle) {
    CHECK_HANDLE(handle);
    close_cb_called++;
}


static void sv_send_cb(uv_udp_send_t* req, int status) {
    ASSERT(req != NULL);
    ASSERT(status == 0);
    CHECK_HANDLE(req->handle);

    sv_send_cb_called++;

    // uv_close((uv_handle_t*) req->handle, close_cb);
}


static void cl_recv_cb(uv_udp_t* handle,
                       ssize_t nread,
                       const uv_buf_t* buf,
                       const struct sockaddr* addr,
                       unsigned flags) {
    // CHECK_HANDLE(handle);
    // ASSERT(flags == 0);

    printf("nread=%d\n", nread);

    cl_recv_cb_called++;

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
    cerr << "s=" << s << endl;

    /* we are done with the client handle, we can close it */
    // uv_close((uv_handle_t*) &client, close_cb);
}


int main( int argc, char** argv ) {

    if( std::string( argv[1] ) == "0" )
    {
        int r;
        uv_udp_send_t req;
        uv_buf_t buf;
        struct sockaddr_in addr;

        ASSERT(0 == uv_ip4_addr("0.0.0.0", 49800, &addr));

        r = uv_udp_init(uv_default_loop(), &server);
        ASSERT(r == 0);

        r = uv_udp_init(uv_default_loop(), &client);
        ASSERT(r == 0);

        /* bind to the desired port */
        r = uv_udp_bind(&client, (const struct sockaddr*) &addr, UV_UDP_REUSEADDR);
        ASSERT(r == 0);

        r = uv_udp_set_membership(&client, "225.0.0.37", getInterfaceIP( argv[2] ).c_str(), UV_JOIN_GROUP);
        if (r == UV_ENODEV)
            printf("No multicast support.\n");
        ASSERT(r == 0);
        // r = uv_udp_set_multicast_interface( &client, getInterfaceIP( argv[2] ).c_str() );
        ASSERT( r== 0 );
        // r = uv_udp_set_multicast_loop( &client, 0 );
        ASSERT( r == 0 );
        r = uv_udp_recv_start(&client, alloc_cb, cl_recv_cb);
        ASSERT(r == 0);

        /*
        buf = uv_buf_init("PING", 4);
        //
        r = uv_udp_send(&req,
                        &server,
                        &buf,
                        1,
                        (const struct sockaddr*) &addr,
                        sv_send_cb);
        ASSERT(r == 0);
        */

        /* run the loop till all events are processed */
        uv_run(uv_default_loop(), UV_RUN_DEFAULT);

        //  ASSERT(cl_recv_cb_called == 1);
        //  ASSERT(sv_send_cb_called == 1);
        //  ASSERT(close_cb_called == 2);
    }
    else
    {

        novadem::link::UDPTransmitter utt( 49800, "225.0.0.37", argv[2] );
        std::string str("mon message");
        utt.send( str.c_str(), str.size() );
    }
    // MAKE_VALGRIND_HAPPY();
    return 0;
}
