#ifndef _LDK_XINPUT_WIN32_H_
#define _LDK_XINPUT_WIN32_H_

#define XINPUT_GAMEPAD_DPAD_UP	0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN	0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT	0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT	0x0008
#define XINPUT_GAMEPAD_START	0x0010
#define XINPUT_GAMEPAD_BACK	0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB	0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB	0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER	0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER	0x0200
#define XINPUT_GAMEPAD_A	0x1000
#define XINPUT_GAMEPAD_B	0x2000
#define XINPUT_GAMEPAD_X	0x4000
#define XINPUT_GAMEPAD_Y	0x8000

//#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  9000
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30
#define XINPUT_MAX_AXIS_VALUE 32767
#define XINPUT_MIN_AXIS_VALUE -32768
#define XINPUT_MAX_TRIGGER_VALUE 255

namespace ldk
{
	namespace platform 
	{
		//---------------------------------------------------------------------------
		// Input specific structures and
		//---------------------------------------------------------------------------
		typedef struct _XINPUT_GAMEPAD 
		{
			WORD  wButtons;
			BYTE  bLeftTrigger;
			BYTE  bRightTrigger;
			SHORT sThumbLX;
			SHORT sThumbLY;
			SHORT sThumbRX;
			SHORT sThumbRY;
		} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

		typedef struct _XINPUT_STATE 
		{
			DWORD dwPacketNumber;
			XINPUT_GAMEPAD Gamepad;
		} XINPUT_STATE, *PXINPUT_STATE;

#define XINPUT_GET_STATE_FUNC(name) DWORD name(DWORD dwUserIndex, XINPUT_STATE *pState)
		typedef XINPUT_GET_STATE_FUNC(XInputGetStateFunc);
		XInputGetStateFunc* XInputGetState = nullptr;
		XINPUT_GET_STATE_FUNC(XInputGetStateStub)
		{
			return ERROR_DEVICE_NOT_CONNECTED;
		}

		//---------------------------------------------------------------------------
		// Initialize XInput
		//---------------------------------------------------------------------------
		static void ldk_win32_initXInput()
		{
			char* xInputDllName = "xinput1_3.dll";
			HMODULE hXInput = LoadLibraryA(xInputDllName);
			if (!hXInput)
			{				
				xInputDllName = "xinput9_1_0.dll";
				hXInput = LoadLibraryA(xInputDllName);
			}

			if (!hXInput)
			{
				xInputDllName = "xinput1_1.dll";
				hXInput = LoadLibraryA(xInputDllName);
			}

			if (!hXInput)
			{
				LogError("could not initialize XInput. No suitable xinput dll found.");
				return;
			}

			LogInfo("XInput %s initialized.", xInputDllName);
			//get xinput function pointers
			XInputGetState = (XInputGetStateFunc*) GetProcAddress(hXInput, "XInputGetState");
			if (!XInputGetState) XInputGetState = XInputGetStateStub;
		}

	} //namespace platform
} // namespace ldk

#endif	//_LDK_XINPUT_WIN32_H_
