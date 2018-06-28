#include <iostream>
using namespace std;

#include <umn.h>
using namespace umn;

#include <unistd.h>

#include <thread>

void node_1()
{
    Node mynode( NodeAddress::fromInteger( 1 ) );
    // mynode.addTransport( make_shared<UDPTransport>( "127.0.0.1", 12345, 12346 ) );
    mynode.addTransport( make_shared<NNRx>( "ipc:///tmp/node1.ipc" ) );
    mynode.addTransport( make_shared<NNTx>( "ipc:///tmp/node4.ipc" ) );
    mynode.addTransport( make_shared<NNTx>( "ipc:///tmp/node2.ipc" ) );
    mynode.run();
}

void node_2()
{
    Node mynode( NodeAddress::fromInteger( 2 ) );
    // mynode.addTransport( make_shared<NNPairTransport>( "ipc:///tmp/node1.ipc", "ipc:///tmp/node2.ipc" ) );
    mynode.addTransport( make_shared<NNRx>( "ipc:///tmp/node2.ipc" ) );
    mynode.addTransport( make_shared<NNTx>( "ipc:///tmp/node1.ipc" ) );
    mynode.run();
}

void node_3()
{
    Node mynode( NodeAddress::fromInteger( 3 ) );
    mynode.addTransport( make_shared<NNRx>( "ipc:///tmp/node3.ipc" ) );
    mynode.addTransport( make_shared<NNTx>( "ipc:///tmp/node4.ipc" ) );
    mynode.run();
}

void node_4()
{
    Node mynode( NodeAddress::fromInteger( 4 ) );
    mynode.addTransport( make_shared<NNRx>( "ipc:///tmp/node4.ipc" ) );
    mynode.addTransport( make_shared<NNTx>( "ipc:///tmp/node1.ipc" ) );
    mynode.addTransport( make_shared<NNTx>( "ipc:///tmp/node3.ipc" ) );
    mynode.run();
}

int main( int argc, char** argv )
{
    /*
    srand(time(NULL));
    Frame myFrame;
    myFrame.setSender(NodeAddress::fromInteger(1));
    myFrame.setDestination(NodeAddress::fromInteger(2));
    myFrame.setSequenceNumber(7);
    myFrame.setType(Frame::HELLO);
    myFrame.setFlags(16);
    std::vector< uint8_t > data( 10 );
    for( int i = 0; i < data.size(); ++i )
    {
        data[i] = rand() % 256;
    }
    myFrame.setPayload( data );
    myFrame.printBuffer();
    sleep(2);
    */

    int nodeType = atoi( argv[1] );
    cerr << "nodeType=" << nodeType << endl;

    if( nodeType == 0 )
    {
        // NNPairTransport nnp( "ipc:///tmp/polbak.ipc" );
        // for( int k = 0; k < 10; ++k )
        // cerr << "nn_pair receive FD=" << nnp.fd() << endl;

        std::thread th1 = std::thread( node_1 );
        std::thread th2 = std::thread( node_2 );
        while(true)
        {
            sleep(1);
        }
    }
    else
    {
        std::thread th3 = std::thread( node_3 );
        std::thread th4 = std::thread( node_4 );
        while(true)
        {
            sleep(1);
        }
    }

    return 0;
}
