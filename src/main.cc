#include <iostream>

#include <uv.h>

#include "ipinterface.h"
#include "umparser.h"

using namespace std;

int main( int argc, char** argv )
{
    IPInterface popo(0,"wlan0");

    Packet p;
    p.setType( Packet::UMN_BEACON_PKT );
    sole::uuid u = sole::rebuild(255,0);
    sole::uuid u2 = sole::rebuild(0,255);
    cerr << u << endl << u2 << endl;
    p.setSenderId( u );
    p.setRecepientId( u2 );
    p.debugPrint();

    sleep(1);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT );

    return 0;
}
