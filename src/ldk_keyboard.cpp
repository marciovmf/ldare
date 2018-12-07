
namespace ldk
{
	namespace input
	{
		static const platform::KeyboardState* _keyboardState;

		bool getKey(uint16 key)
		{
			return _keyboardState->key[key] & LDK_KEYSTATE_PRESSED;
		}

		bool isKeyDown(uint16 key)
		{
			return _keyboardState->key[key] == (LDK_KEYSTATE_CHANGED | LDK_KEYSTATE_PRESSED);
		}

		bool isKeyUp(uint16 key)
		{
			return _keyboardState->key[key] == LDK_KEYSTATE_CHANGED;
		}

		void keyboardUpdate()
		{
			_keyboardState = ldk::platform::getKeyboardState();
		}
	} //namespace input
} // namespace ldk
