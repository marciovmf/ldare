
#ifndef _LDK_DEBUG_H_
#define _LDK_DEBUG_H_

#ifdef _LDK_DEBUG_
#include <stdio.h>
	#define LogMsg(prefix, msg, ...) do {printf("\n%s - ", prefix); printf(msg, __VA_ARGS__);} while(0)
	#define LogMsgAndFileLine(prefix, msg, ...) do {printf("\n%s - ", prefix); printf(msg, __VA_ARGS__); printf("@ %s:%d", __FILE__, __LINE__); } while(0)
	#define LogInfo(msg, ...) LogMsg("[INFO]", msg, __VA_ARGS__)
	#define LogWarning(msg, ...) LogMsgAndFileLine("[WARNING]", msg, __VA_ARGS__)
	#define LogError(msg, ...) LogMsgAndFileLine("[ERROR]", msg, __VA_ARGS__)
	#define LDK_ASSERT(condition, msg, ...) do{if (!(condition)) { LogMsgAndFileLine("[Assertion Failed]", msg, __VA_ARGS__); *((int*)0) = 0;} } while(0)
#else
	#define LogMsg(prefix, msg, ...)
	#define LogMsgAndFileLine(prefix, msg, ...)
	#define LogInfo(msg, ...) 
	#define LogWarning(msg, ...) 
	#define LogError(msg, ...) 
	#define LDK_ASSERT(condition, msg, ...)
#endif // _LDK_DEBUG_

#endif // _LDK_DEBUG_H_
