#pragma once

/**
 * @file PlatformDetection.h
 *
 * Defines for which platform (OS/bits/mobile etc) we are compiling for.
 */

#ifdef _WIN32
    #define PBL_PLATFORM_WINDOWS
    #ifdef _WIN64   // 64 bit windows
        #define PBL_PLATFORM_WINDOWS_64
    #else           // 32 bit windows
        #define PBL_PLATFORM_WINDOWS_32
        #error "Unsupported 32-bit windows platform"
    #endif
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE 
        #ifdef TARGET_IPHONE_SIMULATOR
            // simulator
        #endif
        #define PBL_PLATFORM_IOS
        #error "Unsupported IOS platform." 
    #else
        // OSX / macOS
        #define TARGET_OS_OSX 1
        #define PBL_PLATFORM_OSX
    #endif
#elif __ANDROID__
    #define PBL_PLATFORM_ANDROID
	#error "Unsupproted android platform."
#elif __linux
    #if defined _LP64 || defined aarch64 || defined x86_64_|| defined IA64
        #define PBL_PLATFORM_LINUX
    #else
        #error "Unsupported 32-bit linux platform"
    #endif
#elif __unix 
    #error "Unsupported unix platform."  
#elif __posix
    #error "Unsupported posix platform." 
#endif