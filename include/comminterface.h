
#pragma once

#include <memory>
#include <uv.h>

#include "node.h"

class CommInterface
{
public:
    CommInterface( Node* parent );
    virtual ~CommInterface();

private:

protected:
    Node* _parent;

};
