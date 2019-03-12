#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <arpa/inet.h>

#define MAX_FRAME_PAYLOAD_SIZE 1024
#define MAX_FRAME_BUFFER_SIZE MAX_FRAME_PAYLOAD_SIZE + 12
#define FRAME_START_SYMBOL 0x55
#define FRAME_END_SYMBOL 0x4e

namespace umn
{

typedef enum
{
    UMN_FRAME_NONE = 0,
    UMN_FRAME_REVERSE_ROUTING_TABLE
} FrameType;

typedef struct
{
    uint8_t flags;
    uint8_t type;
    uint16_t sender_id;
    uint16_t dest_id;
    uint16_t payload_size;
    uint8_t payload[ MAX_FRAME_PAYLOAD_SIZE ];
    uint16_t crc;
} Frame;

size_t frameToBuffer( Frame& f, uint8_t* buffer, size_t bufferSize );

}
