#pragma once

#include "frame.h"
#include "runnable.h"

namespace monadic
{
    class Node;
    class CommInterface : public novadem::Runnable
    {
        friend class Node;
    public:
        CommInterface(){}
        virtual ~CommInterface(){}

    private:
        void setParentNode( Node* node )
        {
            _parentNode = node;
        }

    protected:
        Node* _parentNode;

    };
}
