// ============================================================================
// File:        debug.c
// Description: Debug logging related functionality (source)
// Author:      Shirobon
// Date:        2023/12/06
// ============================================================================

#include "config.h"

#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static FILE* debug_log;
static uint8_t debug_mode;

#ifdef CONFIG_DEBUG_LOG_ENABLED

bool debug_begin(uint8_t mode)
{
    debug_mode = mode;
    
    printf("\nEmotion by Shirobon ::: Version %s ::: Built on: %s %s\n\n", CONFIG_VERSION, __DATE__, __TIME__);
    
    if(debug_mode & DEBUG_FILE) {
        if((debug_log = fopen(CONFIG_DEBUG_LOG_PATH, "w")) == NULL) {
            printf("[ERROR] Failed to open debug log: %s\n", CONFIG_DEBUG_LOG_PATH);
            return false;
        }
        fprintf(debug_log, "Emotion by Shirobon ::: Version %s ::: Built on: %s %s\n\n", CONFIG_VERSION, __DATE__, __TIME__);
        debug_printf(DEBUG_INFO, "Opened %s for debug logging.\n", CONFIG_DEBUG_LOG_PATH);
    }

    debug_printf(DEBUG_INFO, "Beginning debug log.\n");

    return true;
}

void debug_printf(const char* header, const char* fmt, ...)
{
    va_list argptr;

    if(debug_mode & DEBUG_STDOUT) {
        va_start(argptr, fmt);
        fprintf(stdout, header);
        vfprintf(stdout, fmt, argptr);
        va_end(argptr);
    }

    if(debug_mode & DEBUG_FILE) {
        va_start(argptr, fmt);
        fprintf(debug_log, header);
        vfprintf(debug_log, fmt, argptr);
        va_end(argptr);
    }
}

void debug_end(void)
{
    debug_printf(DEBUG_INFO, "Ending debug log.\n");
    fclose(debug_log);
}

#endif // CONFIG_DEBUG_LOG_ENABLED
