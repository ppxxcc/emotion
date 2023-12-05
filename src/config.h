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
#define CONFIG_FORCEINLINE  // Force inline functions to actually be inlined

#define CONFIG_RESOLUTION 240 // 240 for 240p, 480 for 480i/p


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
