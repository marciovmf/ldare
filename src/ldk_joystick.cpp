
namespace ldk
{
	static const ldk::platform::JoystickState* _joystickState[LDK_MAX_JOYSTICKS];
	namespace input
	{
		int8 getJoystickButton(uint16 key, uint16 index)
		{
			if (index >= LDK_MAX_JOYSTICKS)
				return 0;
			return 	_joystickState[index]->connected && 
				_joystickState[index]->button[key] & LDK_KEYSTATE_PRESSED;
		}

		int8 isJoystickButtonDown(uint16 key, uint16 index)
		{
			if (index >= LDK_MAX_JOYSTICKS)
				return 0;
			//return joystick[index].connected && 
			return	_joystickState[index]->button[key] == (LDK_KEYSTATE_CHANGED | LDK_KEYSTATE_PRESSED);
		}

		int8 isJoystickButtonUp(uint16 key, uint16 index)
		{
			if (index >= LDK_MAX_JOYSTICKS)
				return 0;
			return  _joystickState[index]->connected && _joystickState[index]->button[key] == LDK_KEYSTATE_CHANGED;
		}

		float getJoystickAxis(uint16 axis, uint16 index)
		{
			if (index >= LDK_MAX_JOYSTICKS || !_joystickState[index]->connected)
				return 0.0f;
			return _joystickState[index]->axis[axis];
		}

		void joystickUpdate()
		{
			for (uint32 joystickId = 0; joystickId < LDK_MAX_JOYSTICKS; joystickId++)
			{
				_joystickState[joystickId] = ldk::platform::getJoystickState(joystickId);
			}
		}
	}
}
