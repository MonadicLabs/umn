#pragma once

#include <map>
#include <string>

#include "comminterface.h"
#include "shortid.h"

namespace  umn {

class Node : public umn::Runnable
{
public:

    friend class CommInterface;

    Node( ShortId nodeId = ShortId() )
        :_nodeId(nodeId)
    {

    }

    virtual ~Node(){}

    virtual void run();

    void addCommInterface( CommInterface* cif )
    {
        _cifs.push_back(cif);
        cif->setParentNode( this );
    }

    void addNeighbourReference( ShortId nodeId, CommInterface* source );
    void removeNeighbourReference( ShortId nodeId, CommInterface* source );

    ShortId id()
    {
        return _nodeId;
    }

    std::map< ShortId, std::vector< CommInterface* > > _neighbors;

private:

    std::vector<CommInterface*> _cifs;
    ShortId _nodeId;

protected:

};

}
