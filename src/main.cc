#include <iostream>
using namespace std;

#include <umn.h>
using namespace umn;

#include <unistd.h>

#include <thread>

void node_1()
{
    Node mynode( NodeAddress::fromInteger( 1 ) );
    mynode.addTransport( make_shared<UDPTransport>( "127.0.0.1", 12345, 12346 ) );
    mynode.run();
}

void node_2()
{
    Node mynode( NodeAddress::fromInteger( 2 ) );
    mynode.addTransport( make_shared<UDPTransport>( "127.0.0.1", 12346, 12345 ) );
    mynode.addTransport( make_shared<UDPTransport>( "127.0.0.1", 12347, 12348 ) );
    mynode.run();
}

void node_3()
{
    Node mynode( NodeAddress::fromInteger( 3 ) );
    mynode.addTransport( make_shared<UDPTransport>( "127.0.0.1", 12348, 12347 ) );
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

    std::thread th1 = std::thread( node_1 );
    std::thread th2 = std::thread( node_2 );
    // std::thread th3 = std::thread( node_3 );

    while(true)
    {
        sleep(1);
    }

    return 0;
}
