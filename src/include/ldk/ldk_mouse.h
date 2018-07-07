#ifndef _LDK_MOUSE_H_
#define _LDK_MOUSE_H_

#define LDK_MOUSE_LEFT     	0x00
#define LDK_MOUSE_RIGHT     0x01
#define LDK_MOUSE_MIDDLE    0x02
//#define LDK_MOUSE_X1BUTTON  0x03
//#define LDK_MOUSE_X2BUTTON  0x04

namespace ldk
{
	namespace input
	{
		LDK_API const ldk::Vec2& getMouseCursor();
		LDK_API bool getMouseButton(uint16 mouseButton);
		LDK_API bool getMouseButtonDown(uint16 mouseButton);
		LDK_API bool getMouseButtonUp(uint16 mouseButton);
		LDK_API void mouseUpdate();
	}
}

#endif// _LDK_MOUSE_H_
