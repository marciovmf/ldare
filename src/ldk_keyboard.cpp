
namespace ldk
{
	static const platform::KeyboardState* _keyboardState;

	static int8 ldk_keyboard_getKey(uint16 key)
	{
		return _keyboardState->key[key] & LDK_KEYSTATE_PRESSED;
	}

	static int8 ldk_keyboard_getKeyDown(uint16 key)
	{
		return _keyboardState->key[key] == (LDK_KEYSTATE_CHANGED | LDK_KEYSTATE_PRESSED);
	}

	static int8 ldk_keyboard_getKeyUp(uint16 key)
	{
		return _keyboardState->key[key] == LDK_KEYSTATE_CHANGED;
	}

	inline void ldk_keyboard_update()
	{
		_keyboardState = ldk::platform::getKeyboardState();
	}

	inline void ldk_keyboard_initApi(ldk::KeyboardApi* keyboardApi)
	{
		keyboardApi->getKeyDown = ldk_keyboard_getKeyDown;
		keyboardApi->getKeyUp 	= ldk_keyboard_getKeyUp;
		keyboardApi->getKey 		= ldk_keyboard_getKey;
	}

} // ldk
