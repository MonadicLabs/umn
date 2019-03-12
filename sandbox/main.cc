#include <iostream>
#include <thread>

#define S2_IMPL

#include "umn.h"
#include "naivepoller.h"
#include "udpstream.h"

using namespace umn;

int main( int argc, char** argv )
{

    Poller* p = new NaivePoller();
    UMN u(p);
    Stream * udps = new UDPStream(12345,"lo", "235.0.0.1" );
    u.addStream( udps );

    while(true)
    {
        u.tick();
    }

    return 0;
}
