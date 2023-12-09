// ============================================================================
// File:        config.h
// Description: Global program configuration
// Author:      Shirobon
// Date:        2023/12/04
// ============================================================================

#ifndef CONFIG_H
#define CONFIG_H

#if !defined(__GNUC__)
    #error This demo is meant to be built with gcc (specifically SH4, ELF)
#endif

#if !defined(__STDC_VERSION__)
    #error This compiler is not reporting a C Standard version.
#endif

#if __STDC_VERSION__ < 199901L
    #error This compiler is too old. This program was written for C99 standard.
#endif

// ============================================================================
// Begin Program Configuration
// ============================================================================
#define CONFIG_VERSION            "0.0.1"

#define CONFIG_FORCEINLINE

#define CONFIG_RESOLUTION         480

#define CONFIG_DEBUG_DEFAULT_MODE DEBUG_STDOUT
#define CONFIG_DEBUG_LOG_ENABLED
#define CONFIG_DEBUG_LOG_PATH     "/pc/debug.log"

#define CONFIG_MAX_TEXTURES       32
#define CONFIG_MAX_MODELS         32

#define CONFIG_MATRIX_STACK_SIZE  32


// ============================================================================
// End Program Configuration
// ============================================================================

#ifdef CONFIG_FORCEINLINE
    #define INLINE static inline __attribute__((always_inline))
#else
    #define INLINE static inline
#endif

#if   CONFIG_RESOLUTION == 240
    #define CONFIG_VIDEO_MODE DM_320x240
    #define CONFIG_SCREEN_W (320)
    #define CONFIG_SCREEN_H (240)
#elif CONFIG_RESOLUTION == 480
    #define CONFIG_VIDEO_MODE DM_640x480
    #define CONFIG_SCREEN_W (640)
    #define CONFIG_SCREEN_H (480)
#else
    #error Incorrect resolution configuration. Specify 240 or 480.
#endif



#endif // CONFIG_H
