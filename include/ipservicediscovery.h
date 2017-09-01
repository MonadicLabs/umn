#pragma once

#include <uv.h>
#include "udptransmitter.h"

class IPInterface;
class IpServiceDiscovery
{
public:
    IpServiceDiscovery( IPInterface* parent );
    virtual ~IpServiceDiscovery();

    void start();
    void stop();

    void emitBeacon();

private:
    IPInterface* _parent;

    void initBeaconEmission();
    uv_timer_t _xmitTimer;
    novadem::link::UDPTransmitter * _beaconTransmitter;

    void initBeaconReception();
    uv_udp_t _beaconClient;
    uv_timer_t _ttlTimer;

};
