#pragma once

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <algorithm>
using namespace std;

#include <unistd.h>

#include "transport.h"
#include "parser.h"
#include "iopoller.h"
#include "timer.h"
#include "utils.h"
#include "router.h"
#include "ararouter.h"

namespace umn
{
class Node
{
public:

    static const unsigned int HELLO_PERIOD_MS = 1000;

    Node( NodeAddress addr = NodeAddress::fromInteger(0) )
        :_address(addr), _globalSeqNum(0)
    {
        _router = std::make_shared<ARARouter>(this);
        cerr << "NODE ADDRESS=" << _address.asInteger() << endl;
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
                        routeFrame( t, tparser->next() );
                    }
                }
            }
        }
        // Check for timer, if >= HELLO_PERIOD, broadcast a HELLO message :)
        if( (double)_helloTimer.elapsed() / (double)(rand() % 1000000) > HELLO_PERIOD_MS )
        {
            broadcastHELLO();
            _helloTimer.reset();
        }

    }

    void routeFrame( std::shared_ptr<Transport> sourceTransport, std::shared_ptr< Frame > f )
    {
        // cerr << std::dec << _address.asInteger() << " - " << f->bufferAsPythonString() << endl;


        if( f->getHopCount() == 0 )
        {
            cerr << std::dec << _address.asInteger() << " @@@@@@ direct neighboueojr " << f->getSender().asInteger() << endl;
        }

        if( f->getType() == Frame::HELLO )
        {
            cerr << std::dec << _address.asInteger() << " received HELLO" << endl;
            cerr << std::dec << _address.asInteger() << " - sender " << f->getSender().asInteger() << " - hopcount=" << f->getHopCount() << " type=" << f->getType() << endl;

            /*
            if( f->getHopCount() < 5 )
            {
                f->incrementHops();
                broadcast( f );
            }

            // cerr << std::dec << _address.asInteger() << " received HELLO from " << f->getSender().asInteger() << endl;

            if( _neighbours.find( sourceTransport ) == _neighbours.end() )
            {
                _neighbours.insert( make_pair( sourceTransport, f->getSender() ) );
                cerr << "I [" << std::dec << _address.asInteger() << "]  Discovered new neighbour: " << _neighbours[ sourceTransport ].asInteger() << endl;
                sleep(1);
            }
            return;
            */
        }
        else if( _router )
        {
            _router->processFrame( f, sourceTransport );
        }
    }

    void broadcastHELLO()
    {
        std::shared_ptr< Frame > helloFrame = std::make_shared<Frame>();
        helloFrame->setType( Frame::HELLO );
        helloFrame->setSender( _address );
        helloFrame->updateBuffer();
        broadcast( helloFrame );
    }

    static std::shared_ptr< Node > createFromFile( const std::string& configFilePath )
    {
        return nullptr;
    }

    bool addTransport( std::shared_ptr< Transport > t )
    {
        _transports.push_back( t );
        _parsers.insert( make_pair( t, std::make_shared<Parser>() ) );
        if( _router )
        {
            _router->onTransportAdded( t );
        }
    }

    void send( std::shared_ptr<Frame> f, std::shared_ptr<Transport> t )
    {
        // f->setSender( _address );
        // f->setSequenceNumber( _globalSeqNum++ );
        t->write( f );
    }

    void broadcast( std::shared_ptr<Frame> f, std::vector< std::shared_ptr< Transport > > excludedTransports = std::vector< std::shared_ptr< Transport > >(0) )
    {
        for( std::shared_ptr<Transport> t : _transports )
        {
            if(std::find(excludedTransports.begin(), excludedTransports.end(), t) != excludedTransports.end()) {
                /* v contains x */
            } else {
                /* v does not contain x */
                send( f, t );
            }
        }
    }

private:
    bool removeTransport( std::shared_ptr< Transport > t );

protected:
    std::vector< std::shared_ptr< Transport > > _transports;
    std::map< std::shared_ptr< Transport >, std::shared_ptr< Parser > > _parsers;
    std::shared_ptr< Router > _router;

    IOPoller _poller;
    Timer _helloTimer;

    NodeAddress _address;
    uint16_t _globalSeqNum;

    std::map< std::shared_ptr< Transport >, NodeAddress > _neighbours;

};

}
