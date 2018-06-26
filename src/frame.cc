#include "frame.h"
#include "utils.h"

#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;

#include <unistd.h>
#include <string.h>
#include <byteswap.h>

umn::Frame::Frame(NodeAddress sender, NodeAddress destination)
    :_hops(0)
{
    // Let's assume size of 128
    _buffer.reserve( HEADER_OVERHEAD + FOOTER_OVERHEAD + 128 );
    _buffer.resize( HEADER_OVERHEAD + FOOTER_OVERHEAD );
    _buffer[0] = START_0_SYMBOL;
    _buffer[1] = START_1_SYMBOL;
    _buffer[2] = SYNC_SYMBOL;
    _buffer[3] = VERSION;
}

umn::Frame::~Frame()
{

}

void umn::Frame::setType(umn::Frame::Type type)
{
    _type = type;
    updateBuffer();
}

void umn::Frame::setFlags(uint8_t flags)
{
    _flags = flags;
    updateBuffer();
}

void umn::Frame::setSender(umn::NodeAddress sender)
{
    _sender = sender;
    updateBuffer();
}

void umn::Frame::setDestination(umn::NodeAddress destination)
{
    _destination = destination;
    updateBuffer();
}

void umn::Frame::setSequenceNumber(uint16_t value)
{
    _seqNum = value;
    updateBuffer();
}

void umn::Frame::setPayload(uint8_t *buffer, size_t len)
{
    _payloadLength = len;
    _buffer.resize( HEADER_OVERHEAD + FOOTER_OVERHEAD + _payloadLength );
    memcpy( &_buffer[0] + HEADER_OVERHEAD, buffer, len );
}

void umn::Frame::setPayload(std::vector<uint8_t> buffer)
{
    setPayload( &buffer[0], buffer.size() );
}

void umn::Frame::copyTo(uint8_t *buffer, size_t len)
{
    updateBuffer();
    memcpy( buffer, (uint8_t*)(&_buffer[0]), _buffer.size() );
}

std::string umn::Frame::bufferAsString()
{
    updateBuffer();
    std::stringstream sstr;
    // print_bytes( sstr, "Frame", (char*)(&_buffer[0]), _buffer.size() );
    return sstr.str();
}

std::string umn::Frame::bufferAsPythonString()
{
    updateBuffer();
    std::stringstream sstr;
    for( int i = 0; i < _buffer.size(); ++i )
    {
        sstr << "\\x" << std::setfill('0') << std::setw(2) << std::hex << (int)_buffer[i];
    }
    return sstr.str();
}

void umn::Frame::printBuffer()
{
    std::cout << bufferAsPythonString() << std::endl;
}

void umn::Frame::updateBuffer()
{
    int offset = 6;

    // Update header fields
    _buffer[4] = _type;
    _buffer[5] = _flags;

    _sender.copyTo( (uint8_t*)(&_buffer[0] + offset) );
    offset += sizeof( uint32_t );

    _destination.copyTo( (uint8_t*)(&_buffer[0] + offset ) );
    offset += sizeof( uint32_t );

    uint16_t shops = bs16(_hops);
    memcpy( (uint8_t*)(&_buffer[0] + offset), (uint8_t*)(&shops), sizeof(uint16_t) );
    offset += sizeof( uint16_t );

    uint16_t snum = bs16(_seqNum);
    memcpy( (uint8_t*)(&_buffer[0] + offset), (uint8_t*)(&snum), sizeof(uint16_t) );
    offset += sizeof( uint16_t );

    uint16_t spllen = bs16(_payloadLength);
    // cerr << "pllen=" << _payloadLength << " spllen=" << spllen << endl;
    memcpy( (uint8_t*)(&_buffer[0] + offset), (uint8_t*)(&spllen), sizeof(uint16_t) );
    // print_bytes( cerr, "test0", (uint8_t*)(&_buffer[0] + offset), 2 );
    // sleep(1);

    offset += sizeof( uint16_t );

    //
    _buffer[ _buffer.size() - 1 ] = _crc & 0xFF;
    _buffer[ _buffer.size() - 2 ] = _crc >> 8;

}
