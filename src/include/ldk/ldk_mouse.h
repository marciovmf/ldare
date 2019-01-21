#ifndef _LDK_MOUSE_H_
#define _LDK_MOUSE_H_


namespace ldk
{
	namespace input
	{
    /// @defgroup Mouse Mouse input
    /// @details Mouse input handling
    /// @{

    // bottom left corner is (0,0)
		LDK_API const ldk::Vec2& getMouseCursor();
		LDK_API bool getMouseButton(uint16 mouseButton);
		LDK_API bool getMouseButtonDown(uint16 mouseButton);
		LDK_API bool getMouseButtonUp(uint16 mouseButton);
		LDK_API void mouseUpdate();


    const uint16 LDK_MOUSE_LEFT = 0x00;
    const uint16 LDK_MOUSE_RIGHT = 0x01;
    const uint16 LDK_MOUSE_MIDDLE = 0x02;
//const uint16 LDK_MOUSE_X1BUTTON  = 0x03;
//const uint16 LDK_MOUSE_X2BUTTON  = 0x04;
	}
}

#endif// _LDK_MOUSE_H_
