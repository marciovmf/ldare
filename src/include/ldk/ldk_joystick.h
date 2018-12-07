#ifndef _LDK_JOYSTICK_H_
#define _LDK_JOYSTICK_H_

#define LDK_JOYSTICK_MAX_DIGITAL_BUTTONS 14
#define LDK_JOYSTICK_MAX_AXIS 6
#define LDK_MAX_JOYSTICKS 4

namespace ldk
{
  namespace input
  {
    /// @defgroup Joystick Joystick Input 
    /// @details Joystick input handling
    /// @{
    LDK_API int8 getJoystickButton(uint16 key, uint16 index = 0);
    LDK_API int8 isJoystickButtonDown(uint16 key, uint16 index = 0);
    LDK_API int8 isJoystickButtonUp(uint16 key, uint16 index = 0);
    LDK_API float getJoystickAxis(uint16 axis, uint16 index = 0);
    LDK_API void joystickUpdate();

    // Gamepad ids
    const uint16 LDK_JOYSTICK_1 = 0;
    const uint16 LDK_JOYSTICK_2 = 1;
    const uint16 LDK_JOYSTICK_3 = 2;
    const uint16 LDK_JOYSTICK_4 = 3;


    //---------------------------------------------------------------------------
    // JOYSTICK
    //---------------------------------------------------------------------------
    //digital buttons
    const uint16 LDK_JOYSTICK_DPAD_UP	= 0x00;
    const uint16 LDK_JOYSTICK_DPAD_DOWN	= 0x01;
    const uint16 LDK_JOYSTICK_DPAD_LEFT	= 0x02;
    const uint16 LDK_JOYSTICK_DPAD_RIGHT =	0x03;
    const uint16 LDK_JOYSTICK_START	= 0x04;
    const uint16 LDK_JOYSTICK_FN1	= 0x04;
    const uint16 LDK_JOYSTICK_BACK	= 0x05;
    const uint16 LDK_JOYSTICK_FN2	= 0x05;
    const uint16 LDK_JOYSTICK_LEFT_THUMB	= 0x06;
    const uint16 LDK_JOYSTICK_RIGHT_THUMB	= 0x07;
    const uint16 LDK_JOYSTICK_LEFT_SHOULDER	= 0x08;
    const uint16 LDK_JOYSTICK_RIGHT_SHOULDER	= 0x09;
    const uint16 LDK_JOYSTICK_A	= 0x0A;
    const uint16 LDK_JOYSTICK_BTN1	= 0x0A;
    const uint16 LDK_JOYSTICK_B	= 0x0B;
    const uint16 LDK_JOYSTICK_BTN2 =0x0B;
    const uint16 LDK_JOYSTICK_X	= 0x0C;
    const uint16 LDK_JOYSTICK_BTN3= 0x0C;
    const uint16 LDK_JOYSTICK_Y =	0x0D;
    const uint16 LDK_JOYSTICK_BTN4 = 0x0D;
    // axis
    const uint16 LDK_JOYSTICK_AXIS_LX = 0x00;
    const uint16 LDK_JOYSTICK_AXIS_LY = 0x01;
    const uint16 LDK_JOYSTICK_AXIS_RX = 0x02;
    const uint16 LDK_JOYSTICK_AXIS_RY = 0x03;
    const uint16 LDK_JOYSTICK_AXIS_LTRIGGER = 0x04;
    const uint16 LDK_JOYSTICK_AXIS_RTRIGGER = 0x05;

    /// @}
  }
}
#endif	// _LDK_JOYSTICK_H_
