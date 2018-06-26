#pragma once

#include <vector>
#include <memory>
#include <map>

#include <poll.h>

#include "transport.h"

namespace umn
{
class IOPoller
{
public:
    IOPoller()
    {

    }

    virtual ~IOPoller()
    {

    }

    bool poll( std::vector< std::shared_ptr< Transport > > transports, int timeout_ms = 10 )
    {
        _pfds.clear();
        _transports.clear();
        for( auto t : transports){
            _pfds.push_back({t->fd(), POLLIN, 0});
            _transports.insert( make_pair( t->fd(), t ) );
        }
        int ret = ::poll(&_pfds[0], _pfds.size(), timeout_ms);
        if(ret < 0){
            // TODO
            // throw std::runtime_error(std::string("poll: ") + std::strerror(errno));
        }
        return !!ret;
    }

    std::shared_ptr< Transport > next()
    {
        for(auto &p : _pfds)
        {
            /*
            if(p.revents & (POLLHUP | POLLERR | POLLNVAL)){
                sockets[p.fd]->has_error = true;
                return sockets[p.fd];
            }
            */

            if(p.revents & POLLIN){
                p.revents = 0;
                return _transports[p.fd];
            }

        }

        //signal that we are done with nullptr
        return nullptr;
    }

private:
    using Pollfd = struct pollfd;
    std::vector<Pollfd> _pfds;
    std::map< int, std::shared_ptr< Transport > > _transports;
    std::vector< std::shared_ptr< Transport > > _pollResult;

protected:

};
}
