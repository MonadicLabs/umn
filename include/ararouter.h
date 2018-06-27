#pragma once

#include <map>

#include "router.h"

namespace umn
{
    class ARARouter : public Router
    {
    public:
        ARARouter( Node* parent );
        virtual ~ARARouter();

        virtual void processFrame(std::shared_ptr<Frame> f, std::shared_ptr<Transport> t);

    private:
        bool passRegularFrame( std::shared_ptr< Frame > f, std::shared_ptr< Transport > t );
        bool passForwardFrame( std::shared_ptr< Frame > f, std::shared_ptr< Transport > t );
        bool passBackwardFrame( std::shared_ptr< Frame > f, std::shared_ptr< Transport > t );
        bool passHelloFrame( std::shared_ptr< Frame > f, std::shared_ptr< Transport > t );

    protected:

    };
}
