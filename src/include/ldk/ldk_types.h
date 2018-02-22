#ifndef _LDK_TYPES_H_
#define _LDK_TYPES_H_

// Compile time defines
// plaforms:
// _LDK_WINDOWS_, _LDK_ANDROID_, _LDK_XXX_
//
// Debug options:
// _LDK_DEBUG_
//

#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint8_t uint8;
typedef uint8_t uchar8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef char* ldkString;
typedef void* ldkHandle;

#endif // _LDK_TYPES_H_
