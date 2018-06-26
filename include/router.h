#pragma once

#include "frame.h"
#include "transport.h"

namespace umn
{
    class Node;
    class Router
    {
    public:
        Router( Node * parent )
        {}

        virtual ~Router()
        {}

        virtual void processFrame( std::shared_ptr<Frame> f, std::shared_ptr< Transport > t )
        {

        }

        virtual void onTransportRemoved( std::shared_ptr< Transport > t ){}
        virtual void onTransportAdded( std::shared_ptr< Transport > t ){}

    private:

    protected:

    };
}
