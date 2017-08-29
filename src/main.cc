#include <iostream>
#include <thread>

#include <uv.h>

#include "ipinterface.h"
#include "node.h"
#include "umparser.h"
#include "utils.h"

using namespace std;

void func1( IPInterface* ipf )
{
    while(true)
    {
        Packet p;
        p.setType( Packet::UMN_BEACON_PKT );
        sole::uuid u = sole::rebuild(255,0);
        sole::uuid u2 = sole::rebuild(0,255);
        cerr << u << endl << u2 << endl;
        p.setSenderId( u );
        p.setRecepientId( u2 );
        p.debugPrint();
        cerr << "coucou" << endl;
        ipf->send(p);
        sleep(1);
    }
}

void func2( IPInterface* ipf )
{
    while(true)
    {
        Packet p;
        p.setType( Packet::UMN_BEACON_PKT );
        sole::uuid u = sole::rebuild(255,0);
        sole::uuid u2 = sole::rebuild(0,255);
        cerr << u << endl << u2 << endl;
        p.setSenderId( u );
        p.setRecepientId( u2 );
        p.debugPrint();
        cerr << "coucou" << endl;
        ipf->send(p);
        sleep(1);
    }
}

void func3( Node* nono )
{
    while(true) {
        nono->broadcastBeacon();
        sleep(1);
    }
}

int main( int argc, char** argv )
{
    Node nono;
    cerr << "my beacon: " << nono.getId() << endl;

    std::string dfd = getInterfaceIP( argv[1] );
    cerr << "dfd=" << dfd << endl;

    //    exit(0);
    sleep(2);

    std::shared_ptr<CommInterface> popo = std::make_shared<IPInterface>(&nono, argv[1]);
    std::shared_ptr<CommInterface> popo2 = std::make_shared<IPInterface>(&nono, argv[2]);
    nono.addCommInterface( popo );
    nono.addCommInterface( popo2 );

    sleep(1);

    // std::thread th( func1, popo );
    // std::thread th2( func2, popo2 );
    std::thread th3( func3, &nono );

    uv_run(uv_default_loop(), UV_RUN_DEFAULT );

    return 0;
}
