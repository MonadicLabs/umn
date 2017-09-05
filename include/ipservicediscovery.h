#pragma once

#include <string>
#include <map>

#include <uv.h>
#include "udptransmitter.h"
#include "node.h"

class IPInterface;
class IpServiceDiscovery
{
public:
    IpServiceDiscovery( IPInterface* parent );
    virtual ~IpServiceDiscovery();

    void start();
    void stop();
    void emitBeacon();

    void processBeaconData(uint8_t* buffer, size_t bufferSize , std::string ip);
    void decreaseTTL();

    std::string findNodeId( NodeIdType id );
    std::vector< std::string > getNeighborIPs();

private:
    IPInterface* _parent;

    void initBeaconEmission();
    uv_timer_t _xmitTimer;
    novadem::link::UDPTransmitter * _beaconTransmitter;

    void initBeaconReception();
    uv_udp_t _beaconClient;
    uv_timer_t _ttlTimer;

    typedef struct
    {
        int ttl;
        unsigned long long last_ts;
        std::string ip;
    } IpTimeRef;

    std::map< NodeIdType, IpTimeRef > _ipRefs;

};
