#pragma once

#include <cstdint>
#include <cstddef>

#include "shortid.h"

namespace umn
{
class Frame
{
public:
    static const uint8_t UMN_SYNC_BYTE1 = 'U';
    static const uint8_t UMN_SYNC_BYTE2 = 'M';

public:
    Frame(){}
    Frame( const char* data, int len )
    {

    }

    virtual ~Frame(){}

private:
    uint32_t adler32(unsigned char *data, size_t len);

    uint8_t _syncByte1;
    uint8_t _syncByte2;
    uint8_t _majorProtocolVersion;
    uint8_t _minorProtocolVersion;
    uint8_t _packetType;
    ShortId _srcId;
    ShortId _destId;
    uint32_t _frameId;
    uint8_t _flagsByte;
    uint32_t _crc;

protected:

};
}
