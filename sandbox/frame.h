#pragma once

namespace umn
{

typedef enum
{
    UMN_FRAME_NONE = 0,
    UMN_FRAME_REVERSE_ROUTING_TABLE
} FrameType;

typedef struct
{
    FrameType type;
} Frame;

}
