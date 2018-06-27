#ifndef _LDK_TYPES_H_
#define _LDK_TYPES_H_

// Compile time defines
// plaforms:
// _LDK_WINDOWS_, _LDK_ANDROID_, _LDK_MAC_, _LDK_IOS_
//
// Debug options:
// _LDK_DEBUG_
// Engine/Editor
// _LDK_EDITOR_

#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef char char8;
typedef unsigned char uchar;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef char* ldkSTRING;
typedef void* ldkHANDLE;

typedef uint32 ldkSTATUS;
#define LDK_SUCCESS 1
#define LDK_FAIL 0

// Program return constants
#define LDK_EXIT_SUCCESS 0
#define LDK_EXIT_FAIL 1

#endif // _LDK_TYPES_H_
