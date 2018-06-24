#ifndef _LDK_JOYSTICK_H_
#define _LDK_JOYSTICK_H_

namespace ldk
{
	namespace input
	{
		LDK_API int8 getJoystickButton(uint16 key, uint16 index = 0);
		LDK_API int8 isJoystickButtonDown(uint16 key, uint16 index = 0);
		LDK_API int8 isJoystickButtonUp(uint16 key, uint16 index = 0);
		LDK_API float getJoystickAxis(uint16 axis, uint16 index = 0);
		LDK_API void joystickUpdate();
	}
}

// Gamepad ids
#define LDK_JOYSTICK_1 0
#define LDK_JOYSTICK_2 1
#define LDK_JOYSTICK_3 2
#define LDK_JOYSTICK_4 3

#define LDK_JOYSTICK_MAX_DIGITAL_BUTTONS 14
#define LDK_JOYSTICK_MAX_AXIS 6
#define LDK_MAX_JOYSTICKS 4

	//---------------------------------------------------------------------------
	// JOYSTICK
	//---------------------------------------------------------------------------
	//digital buttons
#define LDK_JOYSTICK_DPAD_UP	0x00
#define LDK_JOYSTICK_DPAD_DOWN	0x01
#define LDK_JOYSTICK_DPAD_LEFT	0x02
#define LDK_JOYSTICK_DPAD_RIGHT	0x03
#define LDK_JOYSTICK_START	0x04
#define LDK_JOYSTICK_FN1	0x04
#define LDK_JOYSTICK_BACK	0x05
#define LDK_JOYSTICK_FN2	0x05
#define LDK_JOYSTICK_LEFT_THUMB	0x06
#define LDK_JOYSTICK_RIGHT_THUMB	0x07
#define LDK_JOYSTICK_LEFT_SHOULDER	0x08
#define LDK_JOYSTICK_RIGHT_SHOULDER	0x09
#define LDK_JOYSTICK_A	0x0A
#define LDK_JOYSTICK_BTN1	0x0A
#define LDK_JOYSTICK_B	0x0B
#define LDK_JOYSTICK_BTN2	0x0B
#define LDK_JOYSTICK_X	0x0C
#define LDK_JOYSTICK_BTN3	0x0C
#define LDK_JOYSTICK_Y	0x0D
#define LDK_JOYSTICK_BTN4	0x0D
	// axis
#define LDK_JOYSTICK_AXIS_LX 0x00
#define LDK_JOYSTICK_AXIS_LY 0x01
#define LDK_JOYSTICK_AXIS_RX 0x02
#define LDK_JOYSTICK_AXIS_RY 0x03
#define LDK_JOYSTICK_AXIS_LTRIGGER 0x04
#define LDK_JOYSTICK_AXIS_RTRIGGER 0x05

#endif	// _LDK_JOYSTICK_H_
