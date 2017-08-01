#pragma once

#include "comminterface.h"

class IPInterface : public CommInterface
{
public:
    IPInterface( Node* parent, const std::string& ifaceName );
    virtual ~IPInterface();

    void printBuffer( uv_buf_t buf );

    void processBeaconData( uint8_t* buffer, size_t bufferSize );

private:
    std::string _ifaceName;

    void initBeaconReception();
    uv_udp_t _beaconClient;

protected:

};
