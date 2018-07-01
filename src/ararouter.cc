
#include "ararouter.h"
#include "node.h"

#include <iostream>
using namespace std;

#include <unistd.h>

umn::ARARouter::ARARouter(umn::Node *parent)
    :Router(parent)
{
    // Reset global timer
    _globalTimer.start();

    //
    _fantTimer.start();

    //
    _helloTimer.start();
}

umn::ARARouter::~ARARouter()
{

}

void umn::ARARouter::processFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<umn::Transport> t)
{
    // ALOG( "frame type: " << f->getType() << endl );

    // Anyway, discard frame coming from ourself
    if( f->getSender() == _parent->address() )
        return;

    switch( f->getType() )
    {
    case Frame::HELLO:
    {
        passHelloFrame( f, t );
        break;
    }

    case Frame::REGULAR:
    {
        break;
    }

    case Frame::FANT:
    {
        // cerr << "received FANT !!!" << endl;
        passForwardFrame( f, t );
        break;
    }

    case Frame::BANT:
    {
        passBackwardFrame( f, t );
        break;
    }
    default:
        break;
    }
}

void umn::ARARouter::tick()
{
    // cerr << "ARARouter of node " << std::dec << _parent->address().asInteger() << endl;

    // Check for timer, if >= HELLO_PERIOD, broadcast a HELLO message :)
    if( (double)_helloTimer.getElapsedTimeInMilliSec() > HELLO_PERIOD_MS )
    {
        _helloTimer.start();
        std::shared_ptr< Frame > helloFrame = std::make_shared<Frame>();
        helloFrame->setType( Frame::HELLO );
        _parent->broadcastSend( helloFrame );
    }
    pruneKnownNodes();

    // Also send FANTs regularly to known hosts...
    if( (double)_fantTimer.getElapsedTimeInMilliSec() > FANT_PERIOD_MS )
    {
        ALOG( "FANT FANT !" << endl );
        _fantTimer.start();
        for( auto kv : _knownNodes )
        {
            sendFANT( kv.first );
        }
        // sendFANT();
    }
}

bool umn::ARARouter::passRegularFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<Transport> t)
{

}

bool umn::ARARouter::passForwardFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<umn::Transport> t)
{
    // If we're the recipient, send a BANT...
    if( f->getDestination() == _parent->address() )
    {
        NodeAddress s = f->getSender();
        ALOG( "we're the recipient ! coming from:" << s.asInteger() << endl );
        // Also write routing information into routing table
        if( _routes.find(s) == _routes.end() )
        {
            ARARoutingEntry rentry;
            rentry.t = t;
            rentry.pheromone = (double)rand() / (double)RAND_MAX;
            ALOG( "addition of route entry for: " << s.asInteger() << " through " << t->label() << endl );
            _routes.insert( make_pair(s, rentry) );
        }
    }
    else
    {
        // ALOG( "we're not the recipient ! " << f->getDestination().asInteger() << endl );
        std::vector< std::shared_ptr< Transport > > excl;
        excl.push_back( t );
        _parent->broadcastRelay( f, excl );
    }
}

bool umn::ARARouter::passBackwardFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<umn::Transport> t)
{

}

bool umn::ARARouter::passHelloFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<umn::Transport> t)
{
    // cerr << "ARARouter of node " << std::dec << _parent->address().asInteger() << " received a HELLO frame." << endl;
    // ALOG( "received a HELLO frame. " << f->getSender().asInteger() << endl );
    // ALOG( "sender: " << f->getSender().asInteger() << "  hops:" << f->getHopCount() << " gtime=" << _globalTimer.getElapsedTimeInMilliSec() << endl; );
    if( _knownNodes.find(f->getSender() ) == _knownNodes.end() )
    {
        _knownNodes.insert( make_pair( f->getSender(), _globalTimer.getElapsedTimeInMilliSec() ) );
        printKnownNodes();
    }
    _knownNodes[ f->getSender() ] = _globalTimer.getElapsedTimeInMilliSec();

    if( f->getHopCount() == 0 )
    {
        if( _neighbours.find( f->getSender() ) == _neighbours.end() )
        {
            // ALOG( "got new neighbour: " << f->getSender().asInteger() << endl );
            _neighbours.insert( make_pair( f->getSender(), t ) );
        }
    }

    if( f->getHopCount() <= HELLO_MAX_HOPS )
    {
        std::vector< std::shared_ptr< Transport > > exclusionList;
        exclusionList.push_back( t );
        _parent->broadcastRelay( f, exclusionList );
    }

}

void umn::ARARouter::printKnownNodes()
{
    stringstream sstr;
    sstr << "known nodes - ";
    for( auto kv : _knownNodes )
    {
        NodeAddress na = kv.first;
        sstr << na.asInteger() << " ";
    }
    ALOG( sstr.str() << endl );
}

void umn::ARARouter::pruneKnownNodes()
{
    for (auto it = _knownNodes.cbegin(); it != _knownNodes.cend() /* not hoisted */; /* no increment */)
    {
        NodeAddress na = it->first;
        double delta_t = _globalTimer.getElapsedTimeInMilliSec() - it->second;
        // if( delta_t > 3000 )
        //    ALOG( "known_node " << na.asInteger() << " delta_t=" << delta_t << endl );
        bool must_delete = delta_t > HELLO_TIMEOUT_MS;
        // bool must_delete = false;
        if (must_delete)
        {
            if( _knownNodes.size() == 1 )
            {
                ALOG( "will delete last node ?? :/ " << delta_t << endl );
            }
            _knownNodes.erase(it++);    // or "it = m.erase(it)" since C++11
            printKnownNodes();
        }
        else
        {
            ++it;
        }
    }
}

void umn::ARARouter::sendFANT( NodeAddress na )
{
    std::shared_ptr< Frame > fant = std::make_shared<Frame>( _parent->address(), na );
    fant->setType( Frame::FANT );
    fant->updateBuffer();
    _parent->broadcastSend( fant );
}
