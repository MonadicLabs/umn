#include "utils.h"

#include <iostream>
#include <iomanip>

void umn::print_bytes(std::ostream &out, const char *title, const unsigned char *data, size_t dataLen, bool format)
{
    out << title << std::endl;
    out << std::setfill('0');
    for(size_t i = 0; i < dataLen; ++i) {
        out << std::hex << std::setw(2) << (int)data[i];
        if (format) {
            out << (((i + 1) % 16 == 0) ? "\n" : " ");
        }
    }
    out << std::endl;
}


uint16_t umn::bs16(uint16_t v)
{
    uint16_t swapped = (v>>8) | (v<<8);
    return swapped;
}


uint32_t umn::bs32(uint32_t num)
{
    uint32_t swapped = ((num>>24)&0xff) | // move byte 3 to byte 0
                        ((num<<8)&0xff0000) | // move byte 1 to byte 2
                        ((num>>8)&0xff00) | // move byte 2 to byte 1
                        ((num<<24)&0xff000000); // byte 0 to byte 3
    return swapped;
}
