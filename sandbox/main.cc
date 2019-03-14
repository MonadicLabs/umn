#include <iostream>
#include <thread>

#define S2_IMPL

#include "umn.h"
#include "naivepoller.h"
#include "udpstream.h"
#include "ipcrxstream.h"
#include "ipctxstream.h"

using namespace umn;

void node1()
{
    Poller* p = new NaivePoller();
    UMN u(1, p);
    // Stream * udps = new UDPStream(12345,"lo", "235.0.0.1" );

    Stream * s12 = new IPCTxStream( "ipc:///tmp/s12.ipc" );
    Stream * s31 = new IPCRxStream( "ipc:///tmp/s31.ipc" );
    Stream * s13 = new IPCTxStream( "ipc:///tmp/s13.ipc" );

    u.addStream( s12 );
    u.addStream( s31 );
    u.addStream( s13 );

    while(true)
    {
        u.tick();
    }
}

void node2()
{
    Poller* p = new NaivePoller();
    UMN u(2, p);
    // Stream * udps = new UDPStream(12345,"lo", "235.0.0.1" );

    Stream * s12 = new IPCRxStream( "ipc:///tmp/s12.ipc" );
    Stream * s23 = new IPCTxStream( "ipc:///tmp/s23.ipc" );
    Stream * s32 = new IPCRxStream( "ipc:///tmp/s32.ipc" );

    u.addStream( s12 );
    u.addStream( s23 );
    u.addStream( s32 );

    while(true)
    {
        u.tick();
    }
}

void node3()
{
    Poller* p = new NaivePoller();
    UMN u(3, p);

    Stream * s32 = new IPCTxStream( "ipc:///tmp/s32.ipc" );
    Stream * s23 = new IPCRxStream( "ipc:///tmp/s23.ipc" );
    Stream * s31 = new IPCTxStream( "ipc:///tmp/s31.ipc" );

    u.addStream( s31 );
    u.addStream( s23 );
    u.addStream( s32 );

    while(true)
    {
        u.tick();
    }
}

int main( int argc, char** argv )
{
    if( argc != 2 )
        return -1;

    int node_id = atoi( argv[1] );
    switch (node_id) {

    case 1 :
        node1();
        break;
    case 2:
        node2();
        break;
    case 3:
        node3();
        break;

    default:
        break;
    }

    return 0;
}
