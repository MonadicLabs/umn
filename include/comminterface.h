#pragma once

#include "frame.h"
#include "runnable.h"

namespace umn
{
    class Node;
    class CommInterface : public umn::Runnable
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
