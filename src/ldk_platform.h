/**
 * ldare_platform.h
 * Defines platform specific functions for abstracting the real OS
 */
#ifndef __LDARE_PLATFORM__
#define __LDARE_PLATFORM__

// Key Modifier
#define LDK_KEY_MOD_SHIFT			0x0001
#define LDK_KEY_MOD_CONTROL   0x0002
#define LDK_KEY_MOD_ALT   		0x0004
#define LDK_KEY_MOD_SUPER   	0x0008

// Key action
#define LDK_KEY_PRESS					0x0001
#define LDK_KEY_REPEAT				0x0002
#define LDK_KEY_RELEASE				0x0003

#include <ldk/ldk.h>

namespace ldk 
{
	namespace platform 
	{
		enum LDKWindowHint
		{
			WIDTH,
			HEIGHT,
			VISIBLE,										// 0 = hidden, > 0 = visible 
			GL_CONTEXT_VERSION_MINOR,
			GL_CONTEXT_VERSION_MAJOR,
			COLOR_BUFFER_BITS,
			DEPTH_BUFFER_BITS
		};

		/* platform specific window */
		struct LDKWindow;

		/* Error callback function */
		typedef void (* LDKPlatformErrorFunc)(uint32 errorCode, const char* errorMsg);

		/* Keyboard key callback function */
		typedef void (* LDKPlatformKeyFunc) (LDKWindow* window, uint32 key, uint32 action, uint32 modifier);
		
		/* Mouse button callback function */
		typedef void(* LDKPlatformMouseButtonFunc) (LDKWindow*, uint32 button, uint32 action, uint32 modifier);

		/* Mouse cursor callback function */
		typedef void(* LDKPlatformMouseCursorFunc) (LDKWindow*, double xPos, double yPos);

		// Initialize the platform layer
		uint32 initialize();

		// terminates the platform layer
		void terminate();

		// Sets error callback for the platform
		void setErrorCallback(LDKPlatformErrorFunc errorCallback);

		// Set the key callback for the given window
		void setKeyCallback(LDKWindow* window, uint32 keyCallback);

		// set the mouse button callback for the given window
		void setMouseButtonCallback(LDKWindow* window, uint32 mouseButtonCallback);

		// Creates a window
		LDKWindow* createWindow(uint32* attributes, const char* title, LDKWindow* share);

		// Toggles the window fullscreen/windowed
		bool toggleFullScreen(LDKWindow* window, bool fullScreen);

		// Destroys a window
		void destroyWindow(LDKWindow* window);

		// returns the value of the close flag of the specified window
		bool windowShouldClose(LDKWindow* window);

		// Displays a window created with LDKWwindowHing::VISIBLE = 0
		void showWindow(LDKWindow* window);

		// Update the window framebuffer
		void swapWindowBuffer(LDKWindow* window);

		// Updates all windows and OS dependent events
		void pollEvents();



		//---------------------------------------------------------------------------
		// Loads an entire file to memory
		//---------------------------------------------------------------------------
		//void* loadFileToBuffer(const char* filename, size_t* bufferSize);

		//---------------------------------------------------------------------------
		// Requests allocation of size bytes of virtual memory
		//---------------------------------------------------------------------------
		//void* memoryAlloc(size_t size);
	
		//---------------------------------------------------------------------------
		// releases allocated virtual memory
		//---------------------------------------------------------------------------
		//void memoryFree(void* memory);
		
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

#endif	// __LDARE_PLATFORM__
