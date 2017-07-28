#ifndef _LDARE_H_
#define _LDARE_H_

#ifdef DEBUG
#	include <iostream>
#	ifdef UNICODE
#		define OUTSTREAM std::wcout
#		define ERRSTREAM std::wcerr
#	else
#		define OUTSTREAM std::cout
#		define ERRSTREAM std::cerr
#	endif // UNICODE
#	define LogMsg(prefix, msg, stdstream) stdstream << prefix << " " << msg << "\n" << __FILE__ << ":" << __LINE__ <<  std::endl
#	define LogInfo(msg) LogMsg("[INFO]", msg, OUTSTREAM)
#	define LogWarning(msg) LogMsg("[WARNING]", msg, OUTSTREAM)
#	define LogError(msg) LogMsg("[ERROR]", msg, OUTSTREAM)
#else
#	define LogMsg(prefix, msg, stdstream)
#	define LogInfo(msg) 
#	define LogWarning(msg) 
#	define LogError(msg) 
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
#endif //_LDARE_H_

#define EXTERN_C extern "C"

#endif // LDARE_ENGINE

