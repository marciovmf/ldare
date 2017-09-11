/**
 * game.h
 * Standard header for ldare based games
 */
#ifndef _LDARE_GAME_
#define _LDARE_GAME_
#include <ldare/ldare.h>
namespace ldare 
{
	namespace game
	{
		//---------------------------------------------------------------------------
		// Runtime settings the game can return to the engine 
		// during initialization
		//---------------------------------------------------------------------------
		struct GameContext
		{
			int32 windowWidth;
			int32 windowHeight;
			size_t gameMemorySize; 				// Amount of memory allocated to the game
			float clearColor[3];
		};

		//---------------------------------------------------------------------------
		// Contains input state for the current frame
		//---------------------------------------------------------------------------
		struct KeyState
		{
			int8 state; 					// 1 if key is down
			int8 thisFrame; 			// number of state transitions during current frame
		};

#define MAX_GAME_KBD_KEYS 255
#define MAX_GAME_MOUSE_KEYS 5
		struct Input
		{
			KeyState keyboard[MAX_GAME_KBD_KEYS];
			KeyState mouse[4];
			struct 
			{
				int32 x;
				int32 y;
			} cursor;
		};

	}// game namespace

	struct Vec3
	{
		float x;
		float y;
		float z;
	};

	struct Rect
	{
		float x;
		float y;
		float width;
		float height;
	};

} // ldare namespace


//---------------------------------------------------------------------------
// Game Interface
// This interface is used by the engine to communicate with the game
//---------------------------------------------------------------------------
extern "C" 
{
	ldare::game::GameContext gameInit();
	void gameStart(void* gameMemory);
	void gameUpdate(const ldare::game::Input& input);
	void gameStop();
}

//sdk includes
#include "ldare_render.h"

//---------------------------------------------------------------------------
// KEYBOARD KEYS / MOSUE BUTTONS macros
//---------------------------------------------------------------------------
#define MOUSE_LBUTTON     0x00
#define MOUSE_RBUTTON     0x01
#define MOUSE_MBUTTON     0x02
#define MOUSE_X1BUTTON    0x03
#define MOUSE_X2BUTTON    0x04

#define KBD_BACK          0x08
#define KBD_TAB           0x09
#define KBD_CLEAR         0x0C
#define KBD_RETURN        0x0D
#define KBD_SHIFT         0x10
#define KBD_CONTROL       0x11
#define KBD_MENU          0x12
#define KBD_PAUSE         0x13
#define KBD_CAPITAL       0x14
#define KBD_ESCAPE        0x1B
#define KBD_CONVERT       0x1C
#define KBD_NONCONVERT    0x1D
#define KBD_ACCEPT        0x1E
#define KBD_MODECHANGE    0x1F
#define KBD_SPACE         0x20
#define KBD_PRIOR         0x21
#define KBD_NEXT          0x22
#define KBD_END           0x23
#define KBD_HOME          0x24
#define KBD_LEFT          0x25
#define KBD_UP            0x26
#define KBD_RIGHT         0x27
#define KBD_DOWN          0x28
#define KBD_SELECT        0x29
#define KBD_PRINT         0x2A
#define KBD_EXECUTE       0x2B
#define KBD_SNAPSHOT      0x2C
#define KBD_INSERT        0x2D
#define KBD_DELETE        0x2E
#define KBD_HELP          0x2F
// Numbers
#define KBD_0			  0x30
#define KBD_1			  0x31
#define KBD_2			  0x32
#define KBD_3			  0x33
#define KBD_4			  0x34
#define KBD_5			  0x35
#define KBD_6			  0x36
#define KBD_7			  0x37
#define KBD_8			  0x38
#define KBD_9			  0x39
// Letters
#define KBD_A			  0x41
#define KBD_B			  0x42
#define KBD_C			  0x43
#define KBD_D			  0x44
#define KBD_E			  0x45
#define KBD_F			  0x46
#define KBD_G			  0x47
#define KBD_H			  0x48
#define KBD_I			  0x49
#define KBD_J			  0x4A
#define KBD_K			  0x4B
#define KBD_L			  0x4C
#define KBD_M			  0x4D
#define KBD_N			  0x4E
#define KBD_O			  0x4F
#define KBD_P			  0x50
#define KBD_Q			  0x51
#define KBD_R			  0x52
#define KBD_S			  0x53
#define KBD_T			  0x54
#define KBD_U			  0x55
#define KBD_V			  0x56
#define KBD_W			  0x57
#define KBD_X			  0x58
#define KBD_Y			  0x59
#define KBD_Z			  0x5A
// NUMPAD
#define KBD_NUMPAD0       0x60
#define KBD_NUMPAD1       0x61
#define KBD_NUMPAD2       0x62
#define KBD_NUMPAD3       0x63
#define KBD_NUMPAD4       0x64
#define KBD_NUMPAD5       0x65
#define KBD_NUMPAD6       0x66
#define KBD_NUMPAD7       0x67
#define KBD_NUMPAD8       0x68
#define KBD_NUMPAD9       0x69
#define KBD_MULTIPLY      0x6A
#define KBD_ADD           0x6B
#define KBD_SEPARATOR     0x6C
#define KBD_SUBTRACT      0x6D
#define KBD_DECIMAL       0x6E
#define KBD_DIVIDE        0x6F
// FUNCTION KEYS
#define KBD_F1            0x70
#define KBD_F2            0x71
#define KBD_F3            0x72
#define KBD_F4            0x73
#define KBD_F5            0x74
#define KBD_F6            0x75
#define KBD_F7            0x76
#define KBD_F8            0x77
#define KBD_F9            0x78
#define KBD_F10           0x79
#define KBD_F11           0x7A
#define KBD_F12           0x7B
#define KBD_F13           0x7C
#define KBD_F14           0x7D
#define KBD_F15           0x7E
#define KBD_F16           0x7F
#define KBD_F17           0x80
#define KBD_F18           0x81
#define KBD_F19           0x82
#define KBD_F20           0x83
#define KBD_F21           0x84
#define KBD_F22           0x85
#define KBD_F23           0x86
#define KBD_F24           0x87

#endif // _LDARE_GAME_
