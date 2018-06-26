#pragma once

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

    protected:

    };
}
