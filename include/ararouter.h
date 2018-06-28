#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include <set>

#include "router.h"
#include "timer.h"
#include "aralog.h"

#define ALOG( msg )                                               \
{                                                                   \
    std::ostringstream os;                                          \
    os << "ARA_" << _parent->address().asInteger() << " - " << msg;                                                      \
    cout << os.str();          \
}

namespace umn
{
class ARARouter : public Router
{

public:

    static const unsigned int HELLO_PERIOD_MS = 100;
    static const unsigned int HELLO_TIMEOUT_MS = 500;
    static const unsigned int HELLO_MAX_HOPS = 5;

    static const unsigned int FANT_PERIOD_MS = 250;

    ARARouter( Node* parent );
    virtual ~ARARouter();

    virtual void processFrame(std::shared_ptr<Frame> f, std::shared_ptr<Transport> t);
    virtual void tick();

private:
    bool passRegularFrame( std::shared_ptr< Frame > f, std::shared_ptr< Transport > t );
    bool passForwardFrame( std::shared_ptr< Frame > f, std::shared_ptr< Transport > t );
    bool passBackwardFrame( std::shared_ptr< Frame > f, std::shared_ptr< Transport > t );
    bool passHelloFrame( std::shared_ptr< Frame > f, std::shared_ptr< Transport > t );

    void printKnownNodes();
    void pruneKnownNodes();

    void sendFANT(NodeAddress na);

protected:
    Timer _helloTimer;
    Timer _fantTimer;
    Timer _globalTimer;

    std::map< NodeAddress, std::shared_ptr< Transport > > _neighbours;
    std::map< NodeAddress, double > _knownNodes;

};
}
