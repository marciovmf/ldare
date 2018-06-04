#ifndef _LDK_H_
#define _LDK_H_

#ifdef _LDK_ENGINE_
#define LDK_API extern "C" __declspec(dllexport) 
#else
#define LDK_API extern "C" __declspec(dllimport)
#endif

#include "ldk_types.h"
#include "ldk_debug.h"
#include "ldk_math.h"
#include "ldk_keyboard.h"
#include "ldk_joystick.h"
#include "ldk_game.h"
#include "ldk_asset.h"
#include "ldk_render.h"
//#include "ldk_random.h"
#include "ldk_ini.h"
#endif //_LDK_H_


