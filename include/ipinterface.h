#pragma once

#include "comminterface.h"
#include "umparser.h"
#include "udptransmitter.h"

#include <sole/sole.hpp>

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
    void emitBeacon();

private:

    typedef struct
    {
        std::string ip;
        int last_ts;
        int ttl;
    } IpReference;

    std::string _ifaceName;

    void initBeaconReception();
    uv_udp_t _beaconClient;
    uv_timer_t _ttlTimer;
    UMParser _parser;

    void addUdpInterface( const std::string& ip );
    void removeUdpInterface( const std::string& ip );

    std::map< sole::uuid, IpReference > _iprefs;

    void initBeaconEmission();
    uv_timer_t _xmitTimer;
    novadem::link::UDPTransmitter * _beaconTransmitter;

protected:

};
