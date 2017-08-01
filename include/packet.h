#pragma once

class Packet
{
public:

    typedef enum
    {
        UMN_BEACON_PKT = 0
    } Type;

    Packet( Packet::Type& type );

private:
    Packet::Type _type;
};
