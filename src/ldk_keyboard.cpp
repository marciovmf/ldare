
namespace ldk
{
	namespace input
	{
		static const platform::KeyboardState* _keyboardState;

		int8 getKey(uint16 key)
		{
			return _keyboardState->key[key] & LDK_KEYSTATE_PRESSED;
		}

		int8 isKeyDown(uint16 key)
		{
			return _keyboardState->key[key] == (LDK_KEYSTATE_CHANGED | LDK_KEYSTATE_PRESSED);
		}

		int8 isKeyUp(uint16 key)
		{
			return _keyboardState->key[key] == LDK_KEYSTATE_CHANGED;
		}

		void keyboardUpdate()
		{
			_keyboardState = ldk::platform::getKeyboardState();
		}
	} //namespace input
} // namespace ldk
