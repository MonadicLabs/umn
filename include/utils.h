#pragma once

#include <iostream>

namespace umn
{
void print_bytes(std::ostream& out, const char *title, const unsigned char *data, size_t dataLen, bool format = true);
uint16_t bs16(uint16_t v);
uint32_t bs32(uint32_t num);
}
