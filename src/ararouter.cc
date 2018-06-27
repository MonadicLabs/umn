
#include "ararouter.h"

#include <iostream>
using namespace std;

#include <unistd.h>

umn::ARARouter::ARARouter(umn::Node *parent)
    :Router(parent)
{

}

umn::ARARouter::~ARARouter()
{

}

void umn::ARARouter::processFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<umn::Transport> t)
{
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

bool umn::ARARouter::passRegularFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<Transport> t)
{

}

bool umn::ARARouter::passForwardFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<umn::Transport> t)
{

}

bool umn::ARARouter::passBackwardFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<umn::Transport> t)
{

}

bool umn::ARARouter::passHelloFrame(std::shared_ptr<umn::Frame> f, std::shared_ptr<umn::Transport> t)
{

}
