#pragma once

#include <memory>
#include <vector>

#include "nodeaddress.h"

namespace umn
{
class Parser;
class Frame
{
public:
    friend class Parser;
    typedef enum
    {
        TYPE_START = 0,
        HELLO,
        TYPE_END
    } Type;

    Frame( NodeAddress sender = NodeAddress::fromInteger(0),
           NodeAddress destination = NodeAddress::fromInteger(0) );
    virtual ~Frame();

    static const uint8_t START_0_SYMBOL = 0x00;
    static const uint8_t START_1_SYMBOL = 0x01;
    static const uint8_t SYNC_SYMBOL = 0x55;
    static const uint8_t VERSION = 0x01;

    static const uint8_t HEADER_OVERHEAD = 22;
    static const uint8_t FOOTER_OVERHEAD = 2;

    static const uint16_t PAYLOAD_MAX_SIZE = 1500 - (HEADER_OVERHEAD + FOOTER_OVERHEAD);

    NodeAddress getSender()
    {
        return _sender;
    }

    NodeAddress getDestination()
    {
        return _destination;
    }

    NodeAddress getPreviousHop()
    {
        return _previousHop;
    }

    Type getType()
    {
        return _type;
    }

    uint8_t getFlags()
    {
        return _flags;
    }

    uint8_t getId()
    {
        return _id;
    }

    size_t getPayloadLength()
    {
        return _payloadLength;
    }

    uint16_t getCRC()
    {
        return _crc;
    }

    void setType( Type type );
    void setFlags( uint8_t flags );
    void setSender( NodeAddress sender );
    void setDestination( NodeAddress destination );
    void setPreviousHop( NodeAddress previousHop );
    void setId( uint16_t value );

    // Payload modification code here
    void setPayload( uint8_t* buffer, size_t len );
    void setPayload( std::vector< uint8_t > buffer );
    //

    size_t getBufferLength()
    {
        return _buffer.size();
    }

    void copyTo( uint8_t* buffer, size_t len );

    std::string bufferAsString();
    std::string bufferAsPythonString();
    void printBuffer();

private:
    Type    _type;
    uint8_t _flags;

    NodeAddress _sender;
    NodeAddress _destination;
    NodeAddress _previousHop;

    uint16_t _id;
    uint16_t _payloadLength;
    uint16_t _crc;

    std::vector< uint8_t > _buffer;
    void updateBuffer();

};
}
