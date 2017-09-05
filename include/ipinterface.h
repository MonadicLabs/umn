#pragma once

#include "comminterface.h"
#include "umparser.h"
#include "udptransmitter.h"
#include "ipservicediscovery.h"

#include <vector>

#include <uv.h>

class IPInterface : public CommInterface
{

public:
    IPInterface( Node* parent, const std::string& ifaceName );
    virtual ~IPInterface();

    virtual int send(Packet &p);

    std::string getInterfaceName()
    {
        return _ifaceName;
    }

private:
    std::string _ifaceName;

    IpServiceDiscovery * _serviceDiscovery;
    UMParser _parser;

    uv_udp_t _uvRecv;
    void initPacketReception();

protected:

};
