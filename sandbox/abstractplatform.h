#pragma once

#include "stream.h"

namespace umn
{
    class AbstractPlatform
    {
    public:
        AbstractPlatform()
        {

        }

        virtual ~AbstractPlatform()
        {

        }

        virtual void poll( Stream** streams, int num_streams )
        {

        }

    private:

    protected:

    };
}
