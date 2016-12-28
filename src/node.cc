
#include <unistd.h>

#include "node.h"

void monadic::Node::run()
{
    // Start
    for( CommInterface* cif : _cifs )
    {
        cif->start();
    }

    while(!hasToStop())
    {
        sleep(1);
    }

    // Stop
    for( CommInterface* cif : _cifs )
    {
        cif->stop();
    }

}

void monadic::Node::addNeighbourReference(monadic::ShortId nodeId, monadic::CommInterface *source)
{
    if( _neighbors.find(nodeId) == _neighbors.end() )
    {
        std::vector<CommInterface*> ciflist;
        ciflist.push_back(source);
        _neighbors.insert( make_pair(nodeId, ciflist) );
    }
    else
    {
        bool alreadyRefd = false;
        for( CommInterface* cif : _neighbors[ nodeId ] )
        {
            if( cif == source )
            {
                alreadyRefd = true;
                break;
            }
        }
        if( !alreadyRefd )
        {
            _neighbors[ nodeId ].push_back( source );
        }
    }
}

void monadic::Node::removeNeighbourReference(monadic::ShortId nodeId, monadic::CommInterface *source)
{
    if( _neighbors.find(nodeId) == _neighbors.end() )
    {
        // Nothing to do
    }
    else
    {
        int ifIdx = -1;
        for( int k = 0; k < _neighbors[ nodeId ].size(); ++k )
        {
            CommInterface* cif = _neighbors[ nodeId ][k];
            if( cif == source )
            {
                ifIdx = k;
                break;
            }
        }
        if( ifIdx >= 0 )
        {
            _neighbors[ nodeId ].erase( _neighbors[ nodeId ].begin() + ifIdx );
            if( _neighbors[ nodeId ].size() <= 0 )
            {
                _neighbors.erase( nodeId );
            }
        }
    }
}
