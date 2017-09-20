/**
 * ldare.h
 * This header file defines types and macros used by both the engine and the
 * game layer
 */

#ifndef __LDARE_H__
#define __LDARE_H__

#ifdef DEBUG
#	include <stdio.h>
#	ifdef UNICODE
#		define OUTSTREAM std::wcout
#		define ERRSTREAM std::wcerr
#	else
#		define OUTSTREAM std::cout
#		define ERRSTREAM std::cerr
#	endif // UNICODE
#	define LogMsg(prefix, msg, ...) printf("%s %s %d", prefix, __FILE__, __LINE__); printf(msg, __VA_ARGS__)
#	define LogInfo(msg, ...) LogMsg("[INFO]", msg, __VA_ARGS__)
#	define LogWarning(msg, ...) LogMsg("[WARNING]", msg, __VA_ARGS__)
#	define LogError(msg, ...) LogMsg("[ERROR]", msg, __VA_ARGS__)
#define ASSERT(condition, msg, ...) do{if (!(condition)) { LogMsg("[Assertion Failed]", msg, __VA_ARGS__); *((int*)0) = 0;} } while(0)
#else
#	define LogMsg(prefix, msg, stdstream)
#	define LogInfo(msg) 
#	define LogWarning(msg) 
#	define LogError(msg) 
# define ASSERT(condition)
#endif // DEBUG

#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#define KILOBYTE(value) (size_t) ((value) * 1024LL)
#define MEGABYTE(value) (size_t) (KILOBYTE(value) * 1024LL)
#define GIGABYTE(value) (size_t) (MEGABYTE(value) * 1024LL)
#endif //__LDARE_H__

