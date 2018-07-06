#ifndef _LDK_H_
#define _LDK_H_

#if defined(_WIN64) || defined(WIN64)
#	define _LDK_WINDOWS_
#else
#	error Unknown/unsupported platform
#endif // defined(_WIN64) || defined(WIN64)

#ifdef _LDK_ENGINE_
#	define LDK_API __declspec(dllexport) 
#	define LDK_GAME_CALLBACK extern "C" __declspec(dllimport) 
#else
#	define LDK_API __declspec(dllimport)
#	define LDK_GAME_CALLBACK extern "C" __declspec(dllexport) 
#endif

#include "ldk_types.h"
#include "ldk_debug.h"
#include "ldk_math.h"
#include "ldk_keyboard.h"
#include "ldk_joystick.h"
#include "ldk_game.h"
#include "ldk_asset.h"
#include "ldk_renderer.h"
#include "ldk_cfg.h"
//#include "ldk_random.h"
#endif //_LDK_H_


