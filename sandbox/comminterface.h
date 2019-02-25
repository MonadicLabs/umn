
#pragma once

#include <memory>
#include <uv.h>

#include "packet.h"

class Node;
class CommInterface
{
public:
    CommInterface( Node* parent );
    virtual ~CommInterface();

    virtual int send( Packet& p ) = 0;

private:

protected:
    Node* _parent;

};
