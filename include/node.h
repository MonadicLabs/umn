
#pragma once

#include <vector>
#include <memory>

#include <sole/sole.hpp>

#include "comminterface.h"

class Node
{
public:
    Node();
    virtual ~Node();
    sole::uuid getId();

    void addCommInterface( std::shared_ptr< CommInterface > ci );
    void broadcastBeacon();

private:


    sole::uuid _nodeId;
    std::vector< std::shared_ptr< CommInterface > > _interfaces;

};
