#ifndef _LDK_ENGINE_
#define _LDK_ENGINE_
#endif

#include <ldk/ldk.h>

#include "ldk_platform.h"
#include "ldk_memory.h"
#include "ldk_renderer.h"
#include "ldk_renderer_buffer.h"
// implementations
#include "ldk_memory.cpp"
#include "ldk_keyboard.cpp"
#include "ldk_joystick.cpp"
#include "ldk_ini.cpp"
#include "ldk_asset.cpp"
// Opengl renderer dependencies
#include "ldk_renderer_gl.cpp"
#include "ldk_renderer_buffer_gl.cpp"
