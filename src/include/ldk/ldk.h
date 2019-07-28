#ifndef _LDK_H_
#define _LDK_H_

// Compile time defines
// plaforms:
// _LDK_WINDOWS_
//
// Debug options:
// _LDK_DEBUG_
// Engine/Editor
// _LDK_EDITOR_

#if defined(_WIN64) || defined(WIN64)
#define _LDK_WINDOWS_
#else
#error Unknown/unsupported platform
#endif // defined(_WIN64) || defined(WIN64)

#ifdef _LDK_ENGINE_
#define LDK_API __declspec(dllexport) 
#define LDK_GAME_CALLBACK extern "C" __declspec(dllimport) 
#else
#define LDK_API __declspec(dllimport)
#define LDK_GAME_CALLBACK extern "C" __declspec(dllexport) 
#endif // _LDK_ENGINE_

#include <stdio.h>
#include "ldk_types.h"
#include "ldk_string.h"
#include "ldk_debug.h"
#include "ldk_math.h"
#include "ldk_keyboard.h"
#include "ldk_mouse.h"
#include "ldk_joystick.h"
#include "ldk_cfg.h"
#include "ldk_typed_handle.h"
#include "ldk_asset.h"
#include "ldk_audio.h"
#include "ldk_renderer_gl.h"
#include "ldk_sprite_batch.h"
#include "ldk_game.h"

#endif //_LDK_H_

