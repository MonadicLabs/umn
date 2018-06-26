
#include "ararouter.h"

#include <iostream>
using namespace std;

umn::ARARouter::ARARouter(umn::Node *parent)
    :Router(parent)
{

}

umn::ARARouter::~ARARouter()
{

}

void umn::ARARouter::processFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<umn::Transport> t)
{
    if( f->getType() == Frame::HELLO )
    {
        if( f->getHopCount() == 0 )
        {
            cerr << "DIRECT hello from ? " << f->getSender().asInteger() << endl;
        }
    }
}
