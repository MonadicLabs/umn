#pragma once

#include "comminterface.h"
#include "umparser.h"
#include "udptransmitter.h"
#include "ipservicediscovery.h"

#include <map>

class IPInterface : public CommInterface
{

public:
    IPInterface( Node* parent, const std::string& ifaceName );
    virtual ~IPInterface();

    virtual int send(Packet &p);

    void test_send();

    void processBeaconData(uint8_t* buffer, size_t bufferSize , string ip);
    void decreaseTTL();

    std::string getInterfaceName()
    {
        return _ifaceName;
    }

private:

    typedef struct
    {
        std::string ip;
        int last_ts;
        int ttl;
    } IpReference;

    std::string _ifaceName;

    IpServiceDiscovery * _serviceDiscovery;
    UMParser _parser;

    void addUdpInterface( const std::string& ip );
    void removeUdpInterface( const std::string& ip );

    std::map< uint32_t, IpReference > _iprefs;

protected:

};
