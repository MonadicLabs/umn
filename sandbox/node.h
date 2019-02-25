
#pragma once

#include <vector>
#include <memory>

#include "comminterface.h"

typedef uint32_t NodeIdType;

class Node
{
public:
    Node();
    virtual ~Node();
    NodeIdType getId();

    void addCommInterface( std::shared_ptr< CommInterface > ci );
    void broadcastBeacon();

private:


    NodeIdType _nodeId;
    std::vector< std::shared_ptr< CommInterface > > _interfaces;

};
