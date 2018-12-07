///@file
#ifndef _LDK_KEYBOARD_H_
#define _LDK_KEYBOARD_H_

namespace ldk
{
  namespace input
  {
    /// @defgroup Keyboard keyboard Input
    /// @details Keyboard input handling
    /// @{

    LDK_API	bool getKey(uint16 key);
    LDK_API	bool isKeyDown(uint16 key);
    LDK_API	bool isKeyUp(uint16 key);
    LDK_API	void keyboardUpdate();


    //---------------------------------------------------------------------------
    // KEYBOARD 
    //---------------------------------------------------------------------------
    const uint16 LDK_KEY_BACK          = 0x08;
    const uint16 LDK_KEY_TAB         = 0x09;
    const uint16 LDK_KEY_CLEAR       = 0x0C;
    const uint16 LDK_KEY_RETURN      = 0x0D;
    const uint16 LDK_KEY_SHIFT       = 0x10;
    const uint16 LDK_KEY_CONTROL     = 0x11;
    const uint16 LDK_KEY_MENU        = 0x12;
    const uint16 LDK_KEY_PAUSE       = 0x13;
    const uint16 LDK_KEY_CAPITAL     = 0x14;
    const uint16 LDK_KEY_ESCAPE      = 0x1B;
    const uint16 LDK_KEY_CONVERT     = 0x1C;
    const uint16 LDK_KEY_NONCONVERT  = 0x1D;
    const uint16 LDK_KEY_ACCEPT      = 0x1E;
    const uint16 LDK_KEY_MODECHANGE  = 0x1F;
    const uint16 LDK_KEY_SPACE       = 0x20;
    const uint16 LDK_KEY_PRIOR       = 0x21;
    const uint16 LDK_KEY_NEXT        = 0x22;
    const uint16 LDK_KEY_END         = 0x23;
    const uint16 LDK_KEY_HOME        = 0x24;
    const uint16 LDK_KEY_LEFT        = 0x25;
    const uint16 LDK_KEY_UP          = 0x26;
    const uint16 LDK_KEY_RIGHT       = 0x27;
    const uint16 LDK_KEY_DOWN        = 0x28;
    const uint16 LDK_KEY_SELECT      = 0x29;
    const uint16 LDK_KEY_PRINT       = 0x2A;
    const uint16 LDK_KEY_EXECUTE     = 0x2B;
    const uint16 LDK_KEY_SNAPSHOT    = 0x2C;
    const uint16 LDK_KEY_INSERT      = 0x2D;
    const uint16 LDK_KEY_DELETE      = 0x2E;
    const uint16 LDK_KEY_HELP        = 0x2F;
    // Numbers
    const uint16 LDK_KEY_0 = 0x30;
    const uint16 LDK_KEY_1 = 0x31;
    const uint16 LDK_KEY_2 = 0x32;
    const uint16 LDK_KEY_3 = 0x33;
    const uint16 LDK_KEY_4 = 0x34;
    const uint16 LDK_KEY_5 = 0x35;
    const uint16 LDK_KEY_6 = 0x36;
    const uint16 LDK_KEY_7 = 0x37;
    const uint16 LDK_KEY_8 = 0x38;
    const uint16 LDK_KEY_9 = 0x39;
    // Letters
    const uint16 LDK_KEY_A = 0x41;
    const uint16 LDK_KEY_B = 0x42;
    const uint16 LDK_KEY_C = 0x43;
    const uint16 LDK_KEY_D = 0x44;
    const uint16 LDK_KEY_E = 0x45;
    const uint16 LDK_KEY_F = 0x46;
    const uint16 LDK_KEY_G = 0x47;
    const uint16 LDK_KEY_H = 0x48;
    const uint16 LDK_KEY_I = 0x49;
    const uint16 LDK_KEY_J = 0x4A;
    const uint16 LDK_KEY_K = 0x4B;
    const uint16 LDK_KEY_L = 0x4C;
    const uint16 LDK_KEY_M = 0x4D;
    const uint16 LDK_KEY_N = 0x4E;
    const uint16 LDK_KEY_O = 0x4F;
    const uint16 LDK_KEY_P = 0x50;
    const uint16 LDK_KEY_Q = 0x51;
    const uint16 LDK_KEY_R = 0x52;
    const uint16 LDK_KEY_S = 0x53;
    const uint16 LDK_KEY_T = 0x54;
    const uint16 LDK_KEY_U = 0x55;
    const uint16 LDK_KEY_V = 0x56;
    const uint16 LDK_KEY_W = 0x57;
    const uint16 LDK_KEY_X = 0x58;
    const uint16 LDK_KEY_Y = 0x59;
    const uint16 LDK_KEY_Z = 0x5A;
    // NUMPAD
    const uint16 LDK_KEY_NUMPAD0   = 0x60;
    const uint16 LDK_KEY_NUMPAD1   = 0x61;
    const uint16 LDK_KEY_NUMPAD2   = 0x62;
    const uint16 LDK_KEY_NUMPAD3   = 0x63;
    const uint16 LDK_KEY_NUMPAD4   = 0x64;
    const uint16 LDK_KEY_NUMPAD5   = 0x65;
    const uint16 LDK_KEY_NUMPAD6   = 0x66;
    const uint16 LDK_KEY_NUMPAD7   = 0x67;
    const uint16 LDK_KEY_NUMPAD8   = 0x68;
    const uint16 LDK_KEY_NUMPAD9   = 0x69;
    const uint16 LDK_KEY_MULTIPLY  = 0x6A;
    const uint16 LDK_KEY_ADD       = 0x6B;
    const uint16 LDK_KEY_SEPARATOR = 0x6C;
    const uint16 LDK_KEY_SUBTRACT  = 0x6D;
    const uint16 LDK_KEY_DECIMAL   = 0x6E;
    const uint16 LDK_KEY_DIVIDE    = 0x6F;
    // FUNCTION KEYS
    const uint16 LDK_KEY_F1        = 0x70;
    const uint16 LDK_KEY_F2        = 0x71;
    const uint16 LDK_KEY_F3        = 0x72;
    const uint16 LDK_KEY_F4        = 0x73;
    const uint16 LDK_KEY_F5        = 0x74;
    const uint16 LDK_KEY_F6        = 0x75;
    const uint16 LDK_KEY_F7        = 0x76;
    const uint16 LDK_KEY_F8        = 0x77;
    const uint16 LDK_KEY_F9        = 0x78;
    const uint16 LDK_KEY_F10       = 0x79;
    const uint16 LDK_KEY_F11       = 0x7A;
    const uint16 LDK_KEY_F12       = 0x7B;
    const uint16 LDK_KEY_F13       = 0x7C;
    const uint16 LDK_KEY_F14       = 0x7D;
    const uint16 LDK_KEY_F15       = 0x7E;
    const uint16 LDK_KEY_F16       = 0x7F;
    const uint16 LDK_KEY_F17       = 0x80;
    const uint16 LDK_KEY_F18       = 0x81;
    const uint16 LDK_KEY_F19       = 0x82;
    const uint16 LDK_KEY_F20       = 0x83;
    const uint16 LDK_KEY_F21       = 0x84;
    const uint16 LDK_KEY_F22       = 0x85;
    const uint16 LDK_KEY_F23       = 0x86;
    const uint16 LDK_KEY_F24       = 0x87;
    /// @}
  }
}

#endif	// _LDK_KEYBOARD_H_
