/**
 * ldare_platform.h
 * Defines platform specific functions for abstracting the real OS
 */
#ifndef _LDK_PLATFORM_H_
#define _LDK_PLATFORM_H_

#include <ldk/ldk.h>

namespace ldk 
{
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
			KeyState button[LDK_GAMEPAD_MAX_DIGITAL_BUTTONS];
			float axis[LDK_GAMEPAD_MAX_AXIS];
			uint8 connected;
		};

		struct GamepadState
		{
			KeyState button[LDK_GAMEPAD_MAX_DIGITAL_BUTTONS];
			float axis[LDK_GAMEPAD_MAX_AXIS];
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
		uint32 initialize();

		// terminates the platform layer
		void terminate();

		// Sets error callback for the platform
		void setErrorCallback(LDKPlatformErrorFunc errorCallback);

		// sets the close callback for teh giver window
		// the close flag is set befor this callback, but it is possible to override it with setWindowCloseFlag
		void setWindowCloseCallback(LDKWindow* window, LDKPlatformWindowCloseFunc windowCloseCallback);
		
		// sets the reize callback for the given window
		void setWindowResizeCallback(LDKWindow* window, LDKPlatformWindowResizeFunc windowResizeCallback);

		// Creates a window
		LDKWindow* createWindow(uint32* attributes, const char* title, LDKWindow* share);

		// Toggles the window fullscreen/windowed
		void toggleFullScreen(LDKWindow* window, bool fullScreen);
		
		bool isFullScreen(LDKWindow* window);

		// Destroys a window
		void destroyWindow(LDKWindow* window);

		// returns the value of the close flag of the specified window
		bool windowShouldClose(LDKWindow* window);

		// Sets the close flag for a window
		// It is usefull for overrinding a closing window and keep it open
		void setWindowCloseFlag(LDKWindow* window, bool flag);

		// Displays a window created with LDKWwindowHing::VISIBLE = 0
		void showWindow(LDKWindow* window);

		// Update the window framebuffer
		void swapWindowBuffer(LDKWindow* window);

		// Get the state of mouse
		const ldk::platform::MouseState* getMouseState();

		// Get the state of keyboard
		const ldk::platform::KeyboardState* getKeyboardState();

		// Get the state of a gamepad.
		const ldk::platform::GamepadState* getGamepadState(uint32 gamepadId);

		// Updates all windows and OS dependent events
		void pollEvents();

		//---------------------------------------------------------------------------
		// Shared Library loading/unloading
		//---------------------------------------------------------------------------

		ldk::platform::SharedLib* loadSharedLib(char* sharedLibName);

		bool unloadSharedLib(ldk::platform::SharedLib* sharedLib);

		const	void* getFunctionFromSharedLib(const ldk::platform::SharedLib*, const char* function);

		//---------------------------------------------------------------------------
		// Loads an entire file to memory
		//---------------------------------------------------------------------------
		void* loadFileToBuffer(const char8* filename, size_t* bufferSize);

		//---------------------------------------------------------------------------
		// Requests allocation of size bytes of virtual memory
		//---------------------------------------------------------------------------
		void* memoryAlloc(size_t size);
	
		//---------------------------------------------------------------------------
		// releases allocated virtual memory
		//---------------------------------------------------------------------------
		void memoryFree(void* memory);
		
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
