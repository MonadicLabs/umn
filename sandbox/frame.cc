
#include "frame.h"

size_t umn::frameToBuffer(umn::Frame &f, uint8_t *buffer, size_t bufferSize)
{
    int offset = 0;
    int sizeRequirement = (MAX_FRAME_BUFFER_SIZE - MAX_FRAME_PAYLOAD_SIZE) + f.payload_size;
    if( bufferSize < sizeRequirement )
    {
        // Fuck
    }
    else
    {
        buffer[ offset++ ] = FRAME_START_SYMBOL;
        buffer[ offset++ ] = f.flags;
        buffer[ offset++ ] = f.type;
        uint16_t hsid = htons( f.sender_id );
        uint16_t hdid = htons( f.dest_id );
        uint16_t hplen = htons( f.payload_size );

        memcpy( buffer + offset, &hsid, sizeof(uint16_t) );
        offset += sizeof(uint16_t);

        memcpy( buffer + offset, &hdid, sizeof(uint16_t) );
        offset += sizeof(uint16_t);

        memcpy( buffer + offset, &hplen, sizeof(uint16_t) );
        offset += sizeof(uint16_t);

        memcpy( buffer + offset, f.payload, f.payload_size );
        offset += f.payload_size;

        uint16_t hcrc = htons(0x00);
        memcpy( buffer + offset, &hcrc, sizeof(uint16_t) );
        offset += sizeof(uint16_t);

        buffer[ offset++ ] = FRAME_END_SYMBOL;
    }
    return offset;
}
