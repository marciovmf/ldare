#ifndef _LDK_PLATFORM_H_
#define _LDK_PLATFORM_H_

#define LDK_MAX_KBD_KEYS 255
#define LDK_MAX_MOUSE_BUTTONS 3
#define LDK_KEYSTATE_PRESSED  0x01
#define LDK_KEYSTATE_CHANGED  0x02

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
			KeyState button[LDK_MAX_MOUSE_BUTTONS];
			Vec2 cursor;
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
			VISIBLE,										// visible if > 0
			GL_CONTEXT_VERSION_MINOR,
			GL_CONTEXT_VERSION_MAJOR,
			COLOR_BUFFER_BITS,
			DEPTH_BUFFER_BITS
		};

		//---------------------------------------------------------------------------
		// Event callbacks 
		//---------------------------------------------------------------------------
		typedef void(*LDKPlatformErrorFunc)(uint32 errorCode, const char* errorMsg);
		typedef void(*LDKPlatformWindowCloseFunc)(LDKWindow*);
		typedef void(*LDKPlatformWindowResizeFunc)(LDKWindow*, int32 width, int32 height);


		//---------------------------------------------------------------------------
		// Initialization and finalization
		//---------------------------------------------------------------------------
		// Initialize the platform layer
		LDK_API uint32 initialize();

		// terminates the platform layer
		LDK_API void terminate();

		// Sets error callback for the platform
		LDK_API void setErrorCallback(LDKPlatformErrorFunc errorCallback);


		//---------------------------------------------------------------------------
		// Window
		//---------------------------------------------------------------------------
		// sets the close callback for the given window
		// the close flag is set before this callback, but it is possible to override it with setWindowCloseFlag
		LDK_API void setWindowCloseCallback(LDKWindow* window, LDKPlatformWindowCloseFunc windowCloseCallback);
		
		// sets the resize callback for the given window
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

		// Displays a window created with LDKWwindowHint::VISIBLE = 0
		LDK_API void showWindow(LDKWindow* window);

		// Update the window framebuffer
		LDK_API void swapWindowBuffer(LDKWindow* window);

		// Updates all windows and OS dependent events
		LDK_API void pollEvents();

		
		//---------------------------------------------------------------------------
		// Input
		//---------------------------------------------------------------------------
		// Get the state of mouse
		LDK_API const ldk::platform::MouseState* getMouseState();

		// Get the state of keyboard
		LDK_API const ldk::platform::KeyboardState* getKeyboardState();

		// Get the state of a joystick.
		LDK_API const ldk::platform::JoystickState* getJoystickState(uint32 joystickId);


		//---------------------------------------------------------------------------
		// Audio
		//---------------------------------------------------------------------------
		LDK_API uint32 createAudioBuffer(void* fmt, uint32 fmtSize, void* data, uint32 dataSize);

		LDK_API void playAudioBuffer(uint32 audioBufferId);


		//---------------------------------------------------------------------------
		// Shared Library loading/unloading
		//---------------------------------------------------------------------------
		LDK_API ldk::platform::SharedLib* loadSharedLib(char* sharedLibName);

		LDK_API bool unloadSharedLib(ldk::platform::SharedLib* sharedLib);

		LDK_API const void* getFunctionFromSharedLib(const ldk::platform::SharedLib*, const char* function);


		//---------------------------------------------------------------------------
		// File and filesystem
		//---------------------------------------------------------------------------
    
    // Loads a whole file to a buffer
		LDK_API void* loadFileToBuffer(const char* fileName, size_t* fileSize);

		LDK_API void* loadFileToBufferOffset(const char* fileName, size_t* fileSize, size_t additionalSize, size_t offset);

		LDK_API int64 getFileWriteTime(const char* fileName);
		
		LDK_API bool copyFile(const char* sourceFileName, const char* destFileName);

		LDK_API bool moveFile(const char* sourceFileName, const char* destFileName);
		
		LDK_API bool deleteFile(const char* sourceFileName);


		//---------------------------------------------------------------------------
		// Memory allocation
		//---------------------------------------------------------------------------
		LDK_API void* memoryAlloc(size_t size);

		LDK_API void memoryFree(void* memory);
		
    LDK_API void* memoryRealloc(void* memory);

		//---------------------------------------------------------------------------
		// Time
		//---------------------------------------------------------------------------
		// Get the number of ticks since engine initialization
		LDK_API uint64 getTicks(); 

		// Get the time in seconds between 2 tick intervals
		LDK_API float getTimeBetweenTicks(uint64 start, uint64 end); 
	}
}
#endif // _LDK_PLATFORM_H_
