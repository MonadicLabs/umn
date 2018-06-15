#pragma once

#include <vector>
#include <map>
#include <memory>
#include <iostream>
using namespace std;

#include "transport.h"
#include "parser.h"
#include "iopoller.h"
#include "timer.h"
#include "utils.h"

namespace umn
{
class Node
{
public:

    static const unsigned int HELLO_PERIOD_MS = 1000;

    Node()
    {

    }

    virtual ~Node()
    {

    }

    void run()
    {
        while(true)
        {
            tick();
        }
    }

    void tick()
    {
        std::shared_ptr<Transport> t;
        if( _poller.poll( _transports ) )
        {
            while( (t = _poller.next()) != nullptr )
            {
                // Got new data from one of our transport.

                // Grab the data
                size_t buffer_len = 256;
                uint8_t buffer[ buffer_len ];
                int r = t->read( buffer, buffer_len );

                // print_bytes( cerr, "raw_buffer", buffer, r );

                // Parse it !
                std::shared_ptr< Parser > tparser = nullptr;
                if( _parsers.find( t ) != _parsers.end() && r > 0 )
                {
                    tparser = _parsers[ t ];
                    tparser->parse( buffer, r );
                    while( tparser->hasNext() )
                    {
                        routeFrame( tparser->next() );
                    }
                }
            }
        }
        // Check for timer, if >= HELLO_PERIOD, broadcast a HELLO message :)
        if( (double)_helloTimer.elapsed() / 1000000.0 > HELLO_PERIOD_MS )
        {
            broadcastHELLO();
            _helloTimer.reset();
        }

    }

    void routeFrame( std::shared_ptr< Frame > f )
    {
        // cerr << "routeFrame()" << " sender:" << f->getSender().asInteger() << " - destination:" << f->getDestination().asInteger() << " - previous:" << f->getPreviousHop().asInteger() << endl;
        // cerr << "pllen=" << f->getPayloadLength() << endl;
        // cerr << f->bufferAsPythonString() << endl;
    }

    void broadcastHELLO()
    {
        cerr << "HELLO!" << endl;
    }

    static std::shared_ptr< Node > createFromFile( const std::string& configFilePath )
    {
        return nullptr;
    }

    bool addTransport( std::shared_ptr< Transport > t )
    {
        _transports.push_back( t );
        _parsers.insert( make_pair( t, std::make_shared<Parser>() ) );
    }

private:
    bool removeTransport( std::shared_ptr< Transport > t );

protected:
    std::vector< std::shared_ptr< Transport > > _transports;
    std::map< std::shared_ptr< Transport >, std::shared_ptr< Parser > > _parsers;
    IOPoller _poller;
    Timer _helloTimer;
};

}
