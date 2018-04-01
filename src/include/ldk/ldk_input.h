#ifndef __LDK_KEYBOARD__
#define __LDK_KEYBOARD__

namespace ldk
{

#define KEYSTATE_PRESSED  0x01	
#define KEYSTATE_CHANGED  0x02

	typedef int8 KeyState;

// Gamepad ids
#define LDK_GAMEPAD_1 0
#define LDK_GAMEPAD_2 1
#define LDK_GAMEPAD_3 2
#define LDK_GAMEPAD_4 3

#define GAMEPAD_MAX_DIGITAL_BUTTONS 14
#define GAMEPAD_MAX_AXIS 6
#define MAX_GAMEPADS 4

	struct Gamepad 
	{
		KeyState button[GAMEPAD_MAX_DIGITAL_BUTTONS];
		float axis[GAMEPAD_MAX_AXIS];
		uint8 connected;
	};

#define MAX_KBD_KEYS 255
#define MAX_MOUSE_KEYS 5
	struct Input
	{
		KeyState keyboard[MAX_KBD_KEYS];
		Gamepad gamepad[MAX_GAMEPADS];
		KeyState mouse[MAX_MOUSE_KEYS];
		struct 
		{
			int32 x;
			int32 y;
		} cursor;


		inline int8 getKey(uint16 key) const
		{
			return keyboard[key] & (KEYSTATE_CHANGED | KEYSTATE_PRESSED);
		}

		inline int8 getKeyDown(uint16 key) const
		{
			return keyboard[key] == (KEYSTATE_CHANGED | KEYSTATE_PRESSED);
		}

		inline int8 getKeyUp(uint16 key) const
		{
			return keyboard[key] == KEYSTATE_CHANGED;
		}

		inline int8 getButton(uint16 key, uint16 index = 0) const
		{
			if (index >= MAX_GAMEPADS)
				return 0;
			return 	gamepad[index].connected && 
				gamepad[index].button[key] & KEYSTATE_PRESSED;
		}

		inline int8 getButtonDown(uint16 key, uint16 index = 0) const
		{
			if (index >= MAX_GAMEPADS)
				return 0;
			//return gamepad[index].connected && 
			return	gamepad[index].button[key] == (KEYSTATE_CHANGED | KEYSTATE_PRESSED);
		}

		inline int8 getButtonUp(uint16 key, uint16 index = 0) const
		{
			if (index >= MAX_GAMEPADS)
				return 0;
			return  gamepad[index].connected && gamepad[index].button[key] == KEYSTATE_CHANGED;
		}

		inline float getAxis(uint16 axis, uint16 index = 0) const
		{
			if (index >= MAX_GAMEPADS || !gamepad[index].connected)
				return 0.0f;
			return gamepad[index].axis[axis];
		}

	};


	//---------------------------------------------------------------------------
	// GAMEPAD
	//---------------------------------------------------------------------------
	//digital buttons
#define GAMEPAD_DPAD_UP	0x00
#define GAMEPAD_DPAD_DOWN	0x01
#define GAMEPAD_DPAD_LEFT	0x02
#define GAMEPAD_DPAD_RIGHT	0x03
#define GAMEPAD_START	0x04
#define GAMEPAD_FN1	0x04
#define GAMEPAD_BACK	0x05
#define GAMEPAD_FN2	0x05
#define GAMEPAD_LEFT_THUMB	0x06
#define GAMEPAD_RIGHT_THUMB	0x07
#define GAMEPAD_LEFT_SHOULDER	0x08
#define GAMEPAD_RIGHT_SHOULDER	0x09
#define GAMEPAD_A	0x0A
#define GAMEPAD_BTN1	0x0A
#define GAMEPAD_B	0x0B
#define GAMEPAD_BTN2	0x0B
#define GAMEPAD_X	0x0C
#define GAMEPAD_BTN3	0x0C
#define GAMEPAD_Y	0x0D
#define GAMEPAD_BTN4	0x0D
	// axis
#define GAMEPAD_AXIS_LX 0x00
#define GAMEPAD_AXIS_LY 0x01
#define GAMEPAD_AXIS_RX 0x02
#define GAMEPAD_AXIS_RY 0x03
#define GAMEPAD_AXIS_LTRIGGER 0x04
#define GAMEPAD_AXIS_RTRIGGER 0x05

	//---------------------------------------------------------------------------
	// MOSUE 
	//---------------------------------------------------------------------------
#define MOUSE_LBUTTON     0x00
#define MOUSE_RBUTTON     0x01
#define MOUSE_MBUTTON     0x02
#define MOUSE_X1BUTTON    0x03
#define MOUSE_X2BUTTON    0x04

	//---------------------------------------------------------------------------
	// KEYBOARD 
	//---------------------------------------------------------------------------
#define LDK_KEY_BACK          0x08
#define LDK_KEY_TAB           0x09
#define LDK_KEY_CLEAR         0x0C
#define LDK_KEY_RETURN        0x0D
#define LDK_KEY_SHIFT         0x10
#define LDK_KEY_CONTROL       0x11
#define LDK_KEY_MENU          0x12
#define LDK_KEY_PAUSE         0x13
#define LDK_KEY_CAPITAL       0x14
#define LDK_KEY_ESCAPE        0x1B
#define LDK_KEY_CONVERT       0x1C
#define LDK_KEY_NONCONVERT    0x1D
#define LDK_KEY_ACCEPT        0x1E
#define LDK_KEY_MODECHANGE    0x1F
#define LDK_KEY_SPACE         0x20
#define LDK_KEY_PRIOR         0x21
#define LDK_KEY_NEXT          0x22
#define LDK_KEY_END           0x23
#define LDK_KEY_HOME          0x24
#define LDK_KEY_LEFT          0x25
#define LDK_KEY_UP            0x26
#define LDK_KEY_RIGHT         0x27
#define LDK_KEY_DOWN          0x28
#define LDK_KEY_SELECT        0x29
#define LDK_KEY_PRINT         0x2A
#define LDK_KEY_EXECUTE       0x2B
#define LDK_KEY_SNAPSHOT      0x2C
#define LDK_KEY_INSERT        0x2D
#define LDK_KEY_DELETE        0x2E
#define LDK_KEY_HELP          0x2F
	// Numbers
#define LDK_KEY_0			  0x30
#define LDK_KEY_1			  0x31
#define LDK_KEY_2			  0x32
#define LDK_KEY_3			  0x33
#define LDK_KEY_4			  0x34
#define LDK_KEY_5			  0x35
#define LDK_KEY_6			  0x36
#define LDK_KEY_7			  0x37
#define LDK_KEY_8			  0x38
#define LDK_KEY_9			  0x39
	// Letters
#define LDK_KEY_A			  0x41
#define LDK_KEY_B			  0x42
#define LDK_KEY_C			  0x43
#define LDK_KEY_D			  0x44
#define LDK_KEY_E			  0x45
#define LDK_KEY_F			  0x46
#define LDK_KEY_G			  0x47
#define LDK_KEY_H			  0x48
#define LDK_KEY_I			  0x49
#define LDK_KEY_J			  0x4A
#define LDK_KEY_K			  0x4B
#define LDK_KEY_L			  0x4C
#define LDK_KEY_M			  0x4D
#define LDK_KEY_N			  0x4E
#define LDK_KEY_O			  0x4F
#define LDK_KEY_P			  0x50
#define LDK_KEY_Q			  0x51
#define LDK_KEY_R			  0x52
#define LDK_KEY_S			  0x53
#define LDK_KEY_T			  0x54
#define LDK_KEY_U			  0x55
#define LDK_KEY_V			  0x56
#define LDK_KEY_W			  0x57
#define LDK_KEY_X			  0x58
#define LDK_KEY_Y			  0x59
#define LDK_KEY_Z			  0x5A
	// NUMPAD
#define LDK_KEY_NUMPAD0       0x60
#define LDK_KEY_NUMPAD1       0x61
#define LDK_KEY_NUMPAD2       0x62
#define LDK_KEY_NUMPAD3       0x63
#define LDK_KEY_NUMPAD4       0x64
#define LDK_KEY_NUMPAD5       0x65
#define LDK_KEY_NUMPAD6       0x66
#define LDK_KEY_NUMPAD7       0x67
#define LDK_KEY_NUMPAD8       0x68
#define LDK_KEY_NUMPAD9       0x69
#define LDK_KEY_MULTIPLY      0x6A
#define LDK_KEY_ADD           0x6B
#define LDK_KEY_SEPARATOR     0x6C
#define LDK_KEY_SUBTRACT      0x6D
#define LDK_KEY_DECIMAL       0x6E
#define LDK_KEY_DIVIDE        0x6F
	// FUNCTION KEYS
#define LDK_KEY_F1            0x70
#define LDK_KEY_F2            0x71
#define LDK_KEY_F3            0x72
#define LDK_KEY_F4            0x73
#define LDK_KEY_F5            0x74
#define LDK_KEY_F6            0x75
#define LDK_KEY_F7            0x76
#define LDK_KEY_F8            0x77
#define LDK_KEY_F9            0x78
#define LDK_KEY_F10           0x79
#define LDK_KEY_F11           0x7A
#define LDK_KEY_F12           0x7B
#define LDK_KEY_F13           0x7C
#define LDK_KEY_F14           0x7D
#define LDK_KEY_F15           0x7E
#define LDK_KEY_F16           0x7F
#define LDK_KEY_F17           0x80
#define LDK_KEY_F18           0x81
#define LDK_KEY_F19           0x82
#define LDK_KEY_F20           0x83
#define LDK_KEY_F21           0x84
#define LDK_KEY_F22           0x85
#define LDK_KEY_F23           0x86
#define LDK_KEY_F24           0x87

}
#endif // __LDK_KEYBOARD__
