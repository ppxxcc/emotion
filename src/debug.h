// ============================================================================
// File:        debug.h
// Description: Debug logging related functionality (header)
// Author:      Shirobon
// Date:        2023/12/06
// ============================================================================

#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>
#include <stdint.h>

#define DEBUG_STDOUT  0x1
#define DEBUG_FILE    0x2

#define DEBUG_ERROR   "[ERROR] "
#define DEBUG_INFO    "[INFO]  "
#define DEBUG_BLANK   "        "
#define DEBUG_NONE    ""

bool debug_begin(uint8_t mode);
void debug_printf(const char* header, const char* fmt, ...);
void debug_end(void);

#ifndef CONFIG_DEBUG_LOG_ENABLED
    #define debug_begin(x)
    #define debug_printf(x, y, ...)
    #define debug_end()
#endif


#endif // DEBUG_H
