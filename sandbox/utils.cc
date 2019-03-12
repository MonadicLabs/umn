
#include "utils.h"

#include <iostream>
#include <iomanip>
using namespace std;

void print_bytes(std::ostream &out, const char *title, const unsigned char *data, size_t dataLen, bool format, int symbol_per_line) {
    out << title << std::endl;
    out << std::setfill('0');
    for(size_t i = 0; i < dataLen; ++i) {
        out << std::hex << std::setw(2) << (int)data[i];
        if (format) {
            out << (((i + 1) % symbol_per_line == 0) ? "\n" : " ");
        }
    }
    out << std::endl;
}
