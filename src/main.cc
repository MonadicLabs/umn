#include <iostream>
using namespace std;

#include <umn.h>
using namespace umn;

#include <unistd.h>

int main( int argc, char** argv )
{
    srand(time(NULL));
    Frame myFrame;
    myFrame.setSender(NodeAddress::fromInteger(1));
    myFrame.setDestination(NodeAddress::fromInteger(2));
    myFrame.setPreviousHop(NodeAddress::fromInteger(3));
    myFrame.setId(7);
    myFrame.setType(Frame::HELLO);
    myFrame.setFlags(16);
    std::vector< uint8_t > data( Frame::PAYLOAD_MAX_SIZE );
    for( int i = 0; i < data.size(); ++i )
    {
        data[i] = rand() % 256;
    }
    myFrame.setPayload( data );
    myFrame.printBuffer();
    sleep(1);

    Node mynode;
    mynode.addTransport( make_shared<UDPTransport>( "127.0.0.1", 12345 ) );
    mynode.run();

    /*
    IOPoller popol;
    int buffer_len = 512;
    uint8_t buffer[ buffer_len ];
    std::shared_ptr< Transport > t = nullptr;
    Parser p;
    while(true)
    {
        if( popol.poll(_transports, 1000) )
        {
            while( (t = popol.next()) != nullptr )
            {
                int popo = t->read( buffer, buffer_len );
                // cerr << "t=" << t << " - r=" << popo << endl;
                p.parse(buffer, popo);
            }

        }
        // int popo = udpt.read( buffer, buffer_len );
        // cerr << "r=" << popo << endl;
    }
    */
    return 0;
}
