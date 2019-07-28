#ifndef _LDK_TYPES_H_
#define _LDK_TYPES_H_

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

typedef uint32 ldkSTATUS;
#define LDK_SUCCESS 1
#define LDK_FAIL 0

// Program return constants
#define LDK_EXIT_SUCCESS 0
#define LDK_EXIT_FAIL 1


// handle Types
namespace ldk
{
  typedef uint32 Handle;
}

#endif // _LDK_TYPES_H_
