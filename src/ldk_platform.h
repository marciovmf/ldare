/**
 * ldare_platform.h
 * Defines platform specific functions for abstracting the real OS
 */
#ifndef _LDK_PLATFORM_H_
#define _LDK_PLATFORM_H_

#include <ldk/ldk.h>

#define LDK_MAX_KBD_KEYS 255
#define LDK_KEYSTATE_PRESSED  0x01
#define LDK_KEYSTATE_CHANGED  0x02

#if defined(_LDK_WINDOWS_)
#		define LDK_GAME_MODULE_NAME "ldk_game.dll"
#elif defined(_LDK_MAC_) 
#		define LDK_GAME_MODULE_NAME "ldk_game.dll"
#else
#		error "Undefined platform"
#endif

namespace ldk 
{
	typedef int8 KeyState;
	namespace platform 
	{
		// platform specific window 
		struct LDKWindow;

		// platform specific shared library
		struct SharedLib;

		struct KeyboardState
		{
			KeyState key[LDK_MAX_KBD_KEYS];
		};

		struct MouseState
		{
			KeyState button[LDK_JOYSTICK_MAX_DIGITAL_BUTTONS];
			float axis[LDK_JOYSTICK_MAX_AXIS];
			uint8 connected;
		};

		struct JoystickState
		{
			KeyState button[LDK_JOYSTICK_MAX_DIGITAL_BUTTONS];
			float axis[LDK_JOYSTICK_MAX_AXIS];
			uint8 connected;
		};

		enum WindowHint
		{
			WIDTH = 1,
			HEIGHT,
			VISIBLE,										// 0 = hidden, > 0 = visible 
			GL_CONTEXT_VERSION_MINOR,
			GL_CONTEXT_VERSION_MAJOR,
			COLOR_BUFFER_BITS,
			DEPTH_BUFFER_BITS
		};

		// Error callback function 
		typedef void (* LDKPlatformErrorFunc)(uint32 errorCode, const char* errorMsg);

		// Window close callback function 
		typedef void(* LDKPlatformWindowCloseFunc) (LDKWindow*);

		typedef void(* LDKPlatformWindowResizeFunc) (LDKWindow*, int32 width, int32 height);

		// Initialize the platform layer
		LDK_API uint32 initialize();

		// terminates the platform layer
		LDK_API void terminate();

		// Sets error callback for the platform
		LDK_API void setErrorCallback(LDKPlatformErrorFunc errorCallback);

		// sets the close callback for teh giver window
		// the close flag is set befor this callback, but it is possible to override it with setWindowCloseFlag
		LDK_API void setWindowCloseCallback(LDKWindow* window, LDKPlatformWindowCloseFunc windowCloseCallback);
		
		// sets the reize callback for the given window
		LDK_API void setWindowResizeCallback(LDKWindow* window, LDKPlatformWindowResizeFunc windowResizeCallback);

		// Creates a window
		LDK_API LDKWindow* createWindow(uint32* attributes, const char* title, LDKWindow* share);

		// Toggles the window fullscreen/windowed
		LDK_API void toggleFullScreen(LDKWindow* window, bool fullScreen);
		
		LDK_API bool isFullScreen(LDKWindow* window);

		// Destroys a window
		LDK_API void destroyWindow(LDKWindow* window);

		// returns the value of the close flag of the specified window
		LDK_API bool windowShouldClose(LDKWindow* window);

		// Sets the close flag for a window
		// It is usefull for overrinding a closing window and keep it open
		LDK_API void setWindowCloseFlag(LDKWindow* window, bool flag);

		// Displays a window created with LDKWwindowHing::VISIBLE = 0
		LDK_API void showWindow(LDKWindow* window);

		// Update the window framebuffer
		LDK_API void swapWindowBuffer(LDKWindow* window);

		// Get the state of mouse
		LDK_API const ldk::platform::MouseState* getMouseState();

		// Get the state of keyboard
		LDK_API const ldk::platform::KeyboardState* getKeyboardState();

		// Get the state of a gamepad.
		LDK_API const ldk::platform::JoystickState* getJoystickState(uint32 gamepadId);

		// Updates all windows and OS dependent events
		LDK_API void pollEvents();

		//---------------------------------------------------------------------------
		// Shared Library loading/unloading
		//---------------------------------------------------------------------------

		LDK_API ldk::platform::SharedLib* loadSharedLib(char* sharedLibName);

		LDK_API bool unloadSharedLib(ldk::platform::SharedLib* sharedLib);

		LDK_API const	void* getFunctionFromSharedLib(const ldk::platform::SharedLib*, const char* function);

		//---------------------------------------------------------------------------
		// File and filesystem api
		//---------------------------------------------------------------------------
		LDK_API void* loadFileToBuffer(const char8* filename, size_t* bufferSize);

		//---------------------------------------------------------------------------
		// Memory allocation
		//---------------------------------------------------------------------------
		LDK_API void* memoryAlloc(size_t size);
		LDK_API void memoryFree(void* memory);
		
		//---------------------------------------------------------------------------
		// Get the number of ticks since engine initialization
		//---------------------------------------------------------------------------
		//uint64 getTicks(); 

		//---------------------------------------------------------------------------
		// Get the time in seconds between 2 tick intervals
		//---------------------------------------------------------------------------
		//float getTimeBetweenTicks(uint64 start, uint64 end); 
	}
}

#endif	// _LDK_PLATFORM_H_
