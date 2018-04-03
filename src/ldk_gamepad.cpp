
namespace ldk
{
	static const ldk::platform::GamepadState* _gamepadState[LDK_MAX_GAMEPADS];

	static int8 ldk_gamepad_getButton(uint16 key, uint16 index = 0)
	{
		if (index >= LDK_MAX_GAMEPADS)
			return 0;
		return 	_gamepadState[index]->connected && 
			_gamepadState[index]->button[key] & LDK_KEYSTATE_PRESSED;
	}

	static int8 ldk_gamepad_getButtonDown(uint16 key, uint16 index = 0)
	{
		if (index >= LDK_MAX_GAMEPADS)
			return 0;
		//return gamepad[index].connected && 
		return	_gamepadState[index]->button[key] == (LDK_KEYSTATE_CHANGED | LDK_KEYSTATE_PRESSED);
	}

	static int8 ldk_gamepad_getButtonUp(uint16 key, uint16 index = 0)
	{
		if (index >= LDK_MAX_GAMEPADS)
			return 0;
		return  _gamepadState[index]->connected && _gamepadState[index]->button[key] == LDK_KEYSTATE_CHANGED;
	}

	static float ldk_gamepad_getAxis(uint16 axis, uint16 index = 0)
	{
		if (index >= LDK_MAX_GAMEPADS || !_gamepadState[index]->connected)
			return 0.0f;
		return _gamepadState[index]->axis[axis];
	}
	
	inline void ldk_gamepad_update()
	{
		for (uint32 gamepadId = 0; gamepadId < LDK_MAX_GAMEPADS; gamepadId++)
		{
			_gamepadState[gamepadId] = ldk::platform::getGamepadState(gamepadId);
		}
	}

	inline void ldk_gamepad_initApi(ldk::GamepadApi* gamepad)
	{
		gamepad->getButtonDown 	= ldk_gamepad_getButtonDown;
		gamepad->getButtonUp 		= ldk_gamepad_getButtonUp;
		gamepad->getButton 			= ldk_gamepad_getButton;
		gamepad->getAxis 				= ldk_gamepad_getAxis;
	}
}
