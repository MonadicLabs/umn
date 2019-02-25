#pragma once

#include "stream.h"

namespace umn
{

typedef enum
{
    EVENT_NULL = 0,
    EVENT_STREAM_DATA
} EventType;

typedef struct
{
    EventType type;
    Stream* stream;
    uint8_t data;
    size_t data_size;
    void* user_data;
} Event;

}
