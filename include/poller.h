#pragma once

#include "umn.h"

namespace umn
{
    class Poller
    {
    public:
        Poller()
        {

        }

        virtual ~Poller()
        {

        }

        virtual void poll( Stream** streams, int num_streams )
        {

        }

    private:

    protected:

    };
}
