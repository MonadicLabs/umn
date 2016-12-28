
#include <iostream>

#include <unistd.h>

#include "node.h"
#include "comminterface.h"
#include "ipinterface.h"

using namespace std;
using namespace novadem;

int main( int argc, char** argv )
{
    ShortId a;
    ShortId b(a);

    CommInterface* com1 = new novadem::ip::IpInterface(argv[1]);
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

    return 0;
}
