#ifndef _LDK_ENGINE_
#define _LDK_ENGINE_
#endif

#include <ldk/ldk.h>
#include "ldk_memory.h"
#include "ldk_platform.h"
// implementations
#include "ldk_math.cpp"
#include "ldk_memory.cpp"
#include "ldk_keyboard.cpp"
#include "ldk_joystick.cpp"
#include "ldk_asset.cpp"
#include "ldk_cfg.cpp"
// Opengl renderer dependencies
#include "ldk_renderer_gl.cpp"
#include "ldk_renderer_buffer_gl.cpp"

#ifdef _LDK_WINDOWS_
#	include "win32/ldk_platform_win32.cpp"
#endif
