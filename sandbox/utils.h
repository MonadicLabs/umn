#pragma once

#include <iostream>

#if defined(DEBUG) && DEBUG > 0
 #ifdef PLATFORM_ARDUINO
    #define UMN_DEBUG_PRINT(fmt, args...) char popo[1024]; int cx = snprintf(popo, 1024, "DEBUG: %s:%d:%s(): " fmt, \
        __FILE__, __LINE__, __func__, ##args); Serial.write( popo, cx )
 #else
     #define UMN_DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
        __FILE__, __LINE__, __func__, ##args)
 #endif
#else
 #define UMN_DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

void print_bytes(std::ostream& out, const char *title, const unsigned char *data, size_t dataLen, bool format = true, int symbol_per_line = 64 );
