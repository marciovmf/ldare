// LDK engine main compilation unit.

#ifndef _LDK_ENGINE_
#define _LDK_ENGINE_
#endif // _LDK_ENGINE_

#include <ldk/ldk.h>
#include "ldk_platform.h"
#include "ldk_memory.h"


// platform specific implementation
#ifdef _LDK_WINDOWS_
  #include "win32/ldk_platform_win32.cpp"
#else
  #error "Unsupported platform"
#endif

#include "ldk_math.cpp"
#include "ldk_memory.cpp"
#include "ldk_keyboard.cpp"
#include "ldk_mouse.cpp"
#include "ldk_joystick.cpp"
#include "ldk_asset.cpp"
#include "ldk_cfg.cpp"
#include "ldk_sprite_batch.cpp"
#include "ldk_renderer_gl.cpp"
#include "ldk_game.cpp"
