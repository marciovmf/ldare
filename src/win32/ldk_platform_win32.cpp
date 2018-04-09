#ifdef _LDK_WINDOWS_
#define WIN32
#endif // _LDK_WINDOWS_

#include <ldk/ldk.h>
#include "../ldk_platform.h"
#include "../ldk_gl.h"
#include "ldk_xinput_win32.h"
#include "ldk_xaudio2_win32.h"

// Win32 specifics
#include "tchar.h"
#include <windowsx.h>
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#include <objbase.h>

#define LDK_MAX_AUDIO_BUFFER 16
#define LDK_WINDOW_CLASS "LDK_WINDOW_CLASS"

//TODO: Use a custom container with custom memory allocation
#include <map>

namespace ldk 
{
	namespace platform 
	{
		static struct LDKWin32
		{
			LDKPlatformErrorFunc	errorCallback;
			KeyboardState					keyboardState;
			MouseState						mouseState;
			GamepadState					gamepadState[LDK_MAX_GAMEPADS];

			//TODO: add window data to the win32 window instance so there is no need for this map
			std::map<HWND, LDKWindow*> 	windowList;
			uint8 shiftKeyState;
			uint8 controlKeyState;
			uint8 altKeyState;
			uint8 superKeyState;
			BoundAudio boundBufferList[LDK_MAX_AUDIO_BUFFER];
			uint32 boundBufferCount = 0;
		} _platform;

		struct SharedLib
		{
			HMODULE handle;
		};

		/* platform specific window */
		struct LDKWindow
		{
			LDKPlatformWindowCloseFunc	windowCloseCallback;
			HINSTANCE hInstance;
			HWND hwnd;
			HDC dc;
			HGLRC rc;
			bool closeFlag;
			bool fullscreenFlag;
		};

		static HINSTANCE _appInstance;

#define findWindowByHandle(hwnd) (_platform.windowList[hwnd])

		LRESULT CALLBACK ldk_win32_windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch(uMsg)
			{
				case WM_ACTIVATE:
					{
						// reset modifier key state if the window get inactive
						if (LOWORD(wParam) == WA_INACTIVE)
						{
							_platform.shiftKeyState =
								_platform.controlKeyState =
								_platform.altKeyState =
								_platform.superKeyState = 0;
						}
					}
					break;

				case WM_CLOSE:
					{
						LDKWindow* window = findWindowByHandle(hwnd);
						ldk::platform::setWindowCloseFlag(window, true);
					}
					break;

				default:
					return DefWindowProc(hwnd, uMsg, wParam, lParam);	
					break;
			}
			return TRUE;
		}

		static bool ldk_win32_makeContextCurrent(ldk::platform::LDKWindow* window)
		{
			wglMakeCurrent(wglGetCurrentDC(), NULL);
			if (!wglMakeCurrent(window->dc, window->rc))
			{
				//LogError("Could not mmake render context current for window");
				return false;
			}

			return true;
		}

		static bool ldk_win32_registerWindowClass(HINSTANCE hInstance)
		{
			WNDCLASS windowClass = {};
			windowClass.style = CS_OWNDC;
			windowClass.lpfnWndProc = ldk_win32_windowProc;
			windowClass.hInstance = hInstance;
			windowClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
			windowClass.lpszClassName = LDK_WINDOW_CLASS;
			return RegisterClass(&windowClass) != 0;
		}

		static bool ldk_win32_createWindow(
				ldk::platform::LDKWindow* window, uint32 width, uint32 height, HINSTANCE hInstance, TCHAR* title)
		{
			window->hwnd = CreateWindowEx(NULL, 
					TEXT(LDK_WINDOW_CLASS),
					title,
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					width,
					height,
					NULL,
					NULL,
					hInstance,
					NULL);

			if (!window->hwnd)
				return false;

			window->dc = GetDC(window->hwnd);

			return window->dc != NULL;
		}

		static void* ldk_win32_getGlFunctionPointer(const char* functionName)
		{
			static HMODULE opengl32dll = GetModuleHandleA("OpenGL32.dll");
			void* functionPtr = wglGetProcAddress(functionName);
			if( functionPtr == (void*)0x1 || functionPtr == (void*) 0x02 ||
					functionPtr == (void*) 0x3 || functionPtr == (void*) -1 ||
					functionPtr == (void*) 0x0)
			{
				functionPtr = GetProcAddress(opengl32dll, functionName);
				if(!functionPtr)
				{
					LogError("Could not get GL function pointer");
					LogError(functionName);
					return nullptr;
				}
			}

			return functionPtr;
		}

		static bool ldk_win32_initOpenGL(ldk::platform::LDKWindow& gameWindow, HINSTANCE hInstance, int major, int minor, uint32 colorBits = 32, uint32 depthBits = 24)
		{
			ldk::platform::LDKWindow dummyWindow = {};
			if (! ldk_win32_createWindow(&dummyWindow,0,0,hInstance,TEXT("")) )
			{
				LogError("Could not create a dummy window for openGl initialization");
				return false;
			}

			PIXELFORMATDESCRIPTOR pfd = {};
			pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER ;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = colorBits;
			pfd.cDepthBits = depthBits;
			pfd.iLayerType = PFD_MAIN_PLANE;

			int pfId = ChoosePixelFormat(dummyWindow.dc, &pfd);
			if (pfId == 0)
			{
				LogError("Could not find a matching pixel format for GL dummy window");
				return false;
			}

			if (! SetPixelFormat(dummyWindow.dc, pfId, &pfd))
			{
				LogError("Could not set the pixel format for the Gl dummy window");
				return false;
			}

			dummyWindow.rc = wglCreateContext(dummyWindow.dc);
			if (!dummyWindow.rc)
			{
				LogError("Could not create a dummy OpenGl context");
				return false;
			}

			if (!wglMakeCurrent(dummyWindow.dc, dummyWindow.rc))
			{
				LogError("Could not make dummy OpenGL context current");
				return false;
			}

			bool success = true;

#define FETCH_GL_FUNC(type, name) success = success &&\
			(name = (type) ldk::platform::ldk_win32_getGlFunctionPointer((const char*)#name))
			FETCH_GL_FUNC(PFNGLENABLEPROC, glEnable);
			FETCH_GL_FUNC(PFNGLDISABLEPROC, glDisable);
			FETCH_GL_FUNC(PFNGLCLEARPROC, glClear);
			FETCH_GL_FUNC(PFNGLCLEARCOLORPROC, glClearColor);
			FETCH_GL_FUNC(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
			FETCH_GL_FUNC(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB);
			FETCH_GL_FUNC(PFNGLGENBUFFERSPROC, glGenBuffers);
			FETCH_GL_FUNC(PFNGLBINDBUFFERPROC, glBindBuffer);
			FETCH_GL_FUNC(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
			FETCH_GL_FUNC(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
			FETCH_GL_FUNC(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation);
			FETCH_GL_FUNC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
			FETCH_GL_FUNC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
			FETCH_GL_FUNC(PFNGLGETERRORPROC, glGetError);
			FETCH_GL_FUNC(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
			FETCH_GL_FUNC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
			FETCH_GL_FUNC(PFNGLGETSHADERIVPROC, glGetShaderiv);
			FETCH_GL_FUNC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
			FETCH_GL_FUNC(PFNGLCREATESHADERPROC, glCreateShader);
			FETCH_GL_FUNC(PFNGLSHADERSOURCEPROC, glShaderSource);
			FETCH_GL_FUNC(PFNGLCOMPILESHADERPROC, glCompileShader);
			FETCH_GL_FUNC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
			FETCH_GL_FUNC(PFNGLATTACHSHADERPROC, glAttachShader);
			FETCH_GL_FUNC(PFNGLLINKPROGRAMPROC, glLinkProgram);
			FETCH_GL_FUNC(PFNGLDELETESHADERPROC, glDeleteShader);
			FETCH_GL_FUNC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
			FETCH_GL_FUNC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
			FETCH_GL_FUNC(PFNGLBUFFERDATAPROC, glBufferData);
			FETCH_GL_FUNC(PFNGLMAPBUFFERPROC, glMapBuffer);
			FETCH_GL_FUNC(PFNGLUNMAPBUFFERPROC, glUnmapBuffer);
			FETCH_GL_FUNC(PFNGLDRAWELEMENTSPROC, glDrawElements);
			FETCH_GL_FUNC(PFNGLUSEPROGRAMPROC, glUseProgram);
			FETCH_GL_FUNC(PFNGLFLUSHPROC, glFlush);
			FETCH_GL_FUNC(PFNGLVIEWPORTPROC, glViewport);
			FETCH_GL_FUNC(PFNGLGENTEXTURESPROC, glGenTextures);
			FETCH_GL_FUNC(PFNGLBINDTEXTUREPROC, glBindTexture);
			FETCH_GL_FUNC(PFNGLTEXPARAMETERFPROC, glTexParameteri);
			FETCH_GL_FUNC(PFNGLTEXIMAGE2DPROC, glTexImage2D);
			FETCH_GL_FUNC(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);
			FETCH_GL_FUNC(PFNGLBINDBUFFERBASEPROC, glBindBufferBase);
			FETCH_GL_FUNC(PFNGLGETUNIFORMBLOCKINDEXPROC, glGetUniformBlockIndex);
			FETCH_GL_FUNC(PFNGLSCISSORPROC, glScissor);
			FETCH_GL_FUNC(PFNGLDEPTHFUNCPROC, glDepthFunc);
			FETCH_GL_FUNC(PFNGLBLENDFUNCPROC, glBlendFunc);
			FETCH_GL_FUNC(PFNGLDEPTHMASKPROC, glDepthMask);
			FETCH_GL_FUNC(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
			FETCH_GL_FUNC(PFNGLPOLYGONMODEPROC, glPolygonMode);
			FETCH_GL_FUNC(PFNGLPOLYGONOFFSETPROC, glPolygonOffset);
			FETCH_GL_FUNC(PFNGLLINEWIDTHPROC, glLineWidth);

#undef FETCH_GL_FUNC

			if (!success)
			{
				LogError("Could not fetch all necessary OpenGL function pointers");
				return false;
			}

			wglMakeCurrent(0,0);
			wglDeleteContext(dummyWindow.rc);
			DestroyWindow(dummyWindow.hwnd);

			// specify OPENGL attributes for pixel format
			const int pixelFormatAttribList[] =
			{
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_COLOR_BITS_ARB, 32,
				WGL_DEPTH_BITS_ARB, 8,
				0
			};

			pfd = {};
			int numPixelFormats = 0;
			wglChoosePixelFormatARB(
					gameWindow.dc,
					pixelFormatAttribList,
					nullptr,
					1,
					&pfId,
					(UINT*) &numPixelFormats);

			if ( numPixelFormats <= 0)
			{
				LogError("Could not find a matching pixel format");
				return false;
			}

			if (! SetPixelFormat(gameWindow.dc, pfId, &pfd))
			{
				LogError("Could not set pixel format for OpenGL context creation");
				return false;
			}

			const int contextAttribs[] =
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, major,
				WGL_CONTEXT_MINOR_VERSION_ARB, minor,
				WGL_CONTEXT_FLAGS_ARB,
#ifdef _LDK_DEBUG_
				WGL_CONTEXT_DEBUG_BIT_ARB |
#endif // _LDK_DEBUG_
					WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0
			};

			gameWindow.rc = wglCreateContextAttribsARB(
					gameWindow.dc,
					0,
					contextAttribs);

			if (!gameWindow.rc)
			{
				LogError("Could not create a core profile OpenGL context");
				return false;
			}

			if (!ldk_win32_makeContextCurrent(&gameWindow))
				return false;

			return true;
		}

		void ldk_win32_updateGamePad()
		{
			// get gamepad input
			for(int32 gamepadIndex = 0; gamepadIndex < LDK_MAX_GAMEPADS; gamepadIndex++)
			{
				ldk::platform::XINPUT_STATE gamepadState;
				GamepadState& gamepad = _platform.gamepadState[gamepadIndex];

				// ignore unconnected controllers
				if ( platform::XInputGetState(gamepadIndex, &gamepadState) == ERROR_DEVICE_NOT_CONNECTED )
				{
					if ( gamepad.connected)
					{
						gamepad = {};					
					}
					gamepad.connected = 0;
					continue;
				}

				// digital buttons
				WORD buttons = gamepadState.Gamepad.wButtons;
				uint8 isDown=0;
				uint8 wasDown=0;

#define GET_GAMEPAD_BUTTON(btn) do {\
	isDown = (buttons & XINPUT_##btn) > 0;\
	wasDown = gamepad.button[btn] & LDK_KEYSTATE_PRESSED;\
	gamepad.button[btn] = ((isDown != wasDown) << 0x01) | isDown;\
} while(0)
				GET_GAMEPAD_BUTTON(GAMEPAD_DPAD_UP);			
				GET_GAMEPAD_BUTTON(GAMEPAD_DPAD_DOWN);
				GET_GAMEPAD_BUTTON(GAMEPAD_DPAD_LEFT);
				GET_GAMEPAD_BUTTON(GAMEPAD_DPAD_RIGHT);
				GET_GAMEPAD_BUTTON(GAMEPAD_START);
				GET_GAMEPAD_BUTTON(GAMEPAD_BACK);
				GET_GAMEPAD_BUTTON(GAMEPAD_LEFT_THUMB);
				GET_GAMEPAD_BUTTON(GAMEPAD_RIGHT_THUMB);
				GET_GAMEPAD_BUTTON(GAMEPAD_LEFT_SHOULDER);
				GET_GAMEPAD_BUTTON(GAMEPAD_RIGHT_SHOULDER);
				GET_GAMEPAD_BUTTON(GAMEPAD_A);
				GET_GAMEPAD_BUTTON(GAMEPAD_B);
				GET_GAMEPAD_BUTTON(GAMEPAD_X);
				GET_GAMEPAD_BUTTON(GAMEPAD_Y);
#undef SET_GAMEPAD_BUTTON

				//TODO: Make these calculations directly in assembly to make it faster
#define GAMEPAD_AXIS_VALUE(value) (value/(float)(value < 0 ? XINPUT_MIN_AXIS_VALUE * -1: XINPUT_MAX_AXIS_VALUE))
#define GAMEPAD_AXIS_IS_DEADZONE(value, deadzone) ( value > -deadzone && value < deadzone)

				// Left thumb axis
				int32 axisX = gamepadState.Gamepad.sThumbLX;
				int32 axisY = gamepadState.Gamepad.sThumbLY;
				int32 deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

				gamepad.axis[GAMEPAD_AXIS_LX] = GAMEPAD_AXIS_IS_DEADZONE(axisX, deadZone) ? 0.0f :
				GAMEPAD_AXIS_VALUE(axisX);

				gamepad.axis[GAMEPAD_AXIS_LY] = GAMEPAD_AXIS_IS_DEADZONE(axisY, deadZone) ? 0.0f :	
				GAMEPAD_AXIS_VALUE(axisY);

				// Right thumb axis
				axisX = gamepadState.Gamepad.sThumbRX;
				axisY = gamepadState.Gamepad.sThumbRY;
				deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

				gamepad.axis[GAMEPAD_AXIS_RX] = GAMEPAD_AXIS_IS_DEADZONE(axisX, deadZone) ? 0.0f :
				GAMEPAD_AXIS_VALUE(axisX);

				gamepad.axis[GAMEPAD_AXIS_RY] = GAMEPAD_AXIS_IS_DEADZONE(axisY, deadZone) ? 0.0f :	
				GAMEPAD_AXIS_VALUE(axisY);


				// Left trigger
				axisX = gamepadState.Gamepad.bLeftTrigger;
				axisY = gamepadState.Gamepad.bRightTrigger;
				deadZone = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

				gamepad.axis[GAMEPAD_AXIS_LTRIGGER] = GAMEPAD_AXIS_IS_DEADZONE(axisX, deadZone) ? 0.0f :	
				axisX/(float) XINPUT_MAX_TRIGGER_VALUE;

				gamepad.axis[GAMEPAD_AXIS_RTRIGGER] = GAMEPAD_AXIS_IS_DEADZONE(axisY, deadZone) ? 0.0f :	
				axisY/(float) XINPUT_MAX_TRIGGER_VALUE;

#undef GAMEPAD_AXIS_IS_DEADZONE
#undef GAMEPAD_AXIS_VALUE

				gamepad.connected = 1;
				}

}

		//---------------------------------------------------------------------------
		// Plays an audio buffer
		// Returns the created buffer id
		//---------------------------------------------------------------------------
		uint32 ldk_win32_createAudioBuffer(void* fmt, uint32 fmtSize, void* data, uint32 dataSize)
		{
			BoundAudio* audio = nullptr;
			uint32 audioId = _platform.boundBufferCount;
		
			if (_platform.boundBufferCount < LDK_MAX_AUDIO_BUFFER)
			{
				// Get an audio buffer from the list
				audio = &(_platform.boundBufferList[audioId]);
				_platform.boundBufferCount++;
			}
			else
			{
				return -1;
			}
		
			// set format
			WAVEFORMATEXTENSIBLE wfx = *((WAVEFORMATEXTENSIBLE*) fmt);
			// set data
			BYTE *pDataBuffer = (BYTE*) data;
		
			// set XAUDIO2 instructions on what and how to play
			audio->buffer.AudioBytes = dataSize;
			audio->buffer.pAudioData = (BYTE*) data;
			audio->buffer.Flags = XAUDIO2_END_OF_STREAM;
		
			HRESULT hr = 0;
			//TODO: figure out how to use one single struct for both modern and legacy XAudio
			if (pXAudio2_7 != nullptr)
			{
				hr = pXAudio2_7->CreateSourceVoice(&audio->voice, (WAVEFORMATEX*)&wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO,nullptr, nullptr);
			}
			else
			{
				hr = pXAudio2->CreateSourceVoice(&audio->voice, (WAVEFORMATEX*)&wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO,nullptr, nullptr);
			}
			if (FAILED(hr))
			{
				LogError("Error creating source voice");
			}
			return audioId; 
		}

		//---------------------------------------------------------------------------
		// Plays an audio buffer
		//---------------------------------------------------------------------------
		void ldk_win32_playAudio(uint32 audioBufferId)
		{
			if (_platform.boundBufferCount >= LDK_MAX_AUDIO_BUFFER || _platform.boundBufferCount <= 0)
				return;
		
			BoundAudio* audio = &(_platform.boundBufferList[audioBufferId]);
			HRESULT hr = audio->voice->SubmitSourceBuffer(&audio->buffer);
		
			if (FAILED(hr))
			{
				LogError("Error %x submitting audio buffer", hr);
			}
		
			hr = audio->voice->Start(0);
			if (FAILED(hr))
			{
				LogError("Error %x playing audio", hr);
			}
		}

		// Initialize the platform layer
		uint32 initialize()
		{
			CoInitialize(NULL);
			_appInstance = GetModuleHandle(NULL);
		
			ldk_win32_initXInput();
			ldk_win32_initXAudio();
			return ldk_win32_registerWindowClass(_appInstance);
		}

		// terminates the platform layer
		void terminate()
		{
		}

		// Sets error callback for the platform
		void setErrorCallback(LDKPlatformErrorFunc errorCallback)
		{
			_platform.errorCallback = errorCallback;
		}
		
		void setWindowCloseCallback(LDKWindow* window, LDKPlatformWindowCloseFunc windowCloseCallback)
		{
			window->windowCloseCallback = windowCloseCallback;
		}

		// Creates a window
		LDKWindow* createWindow(uint32* attributes, const char* title, LDKWindow* share)
		{
			uint32* pAttribute = attributes;	
			uint32 width = 800;
			uint32 height = 600;
			uint32 visible = 1;
			uint32 colorBits = 32;
			uint32 depthBits = 24;
			uint32 glVersionMajor = 3;
			uint32 glVersionMinor = 0;
			bool success = true;
		
			while ( pAttribute != 0 && *pAttribute != 0 )
			{
				ldk::platform::WindowHint windowHint = (ldk::platform::WindowHint) *pAttribute;
		
				switch (windowHint)
				{
					case ldk::platform::WindowHint::WIDTH:
						width = *++pAttribute;
						break;
					case ldk::platform::WindowHint::HEIGHT:
						height = *++pAttribute;
						break;
					case ldk::platform::WindowHint::VISIBLE:
						visible = *++pAttribute;
						break;
					case ldk::platform::WindowHint::GL_CONTEXT_VERSION_MAJOR:
						glVersionMajor = *++pAttribute;
						break;
					case ldk::platform::WindowHint::GL_CONTEXT_VERSION_MINOR:
						glVersionMinor = *++pAttribute;
						break;
					case ldk::platform::WindowHint::COLOR_BUFFER_BITS:
						colorBits = *++pAttribute;
						break;
					case ldk::platform::WindowHint::DEPTH_BUFFER_BITS:
						depthBits = *++pAttribute;
						break;
					default:
						LogError("Ignoring unkown window hint");
						break;
				}
				++pAttribute;
			}
		
			//TODO Use a custom allocator
			ldk::platform::LDKWindow* window = new LDKWindow();
			*window = {};
		
			if (!ldk_win32_createWindow(window, width, height, _appInstance, (TCHAR*) title))
			{
				LogError("Could not create window");
				return nullptr;
			}
		
			/* create a new context or share an existing one ? */
			if (share)
			{
				//FIXME: Context sharing is not working!
				window->rc = share->rc;
				wglMakeCurrent(window->dc, window->rc);
			}
			else
			{
				if (!ldk_win32_initOpenGL(*window, _appInstance, glVersionMajor, glVersionMinor, colorBits, depthBits))
				{
					success = false;
				}
			}
		
			if (visible)
			{
				ldk::platform::showWindow(window);
			}
		
			if (!success)
			{
				delete window;
				return nullptr;
			}
		
			_platform.windowList.insert(std::make_pair(window->hwnd, window));
			return window;
		}

		// Toggles the window fullscreen/windowed
		bool toggleFullScreen(LDKWindow* window, bool fullScreen)
		{
			return LDK_FAIL;
		}
		
		// Destroys a window
		void destroyWindow(LDKWindow* window)
		{
			auto it = _platform.windowList.find(window->hwnd);
			_platform.windowList.erase(it);
			DestroyWindow(window->hwnd);
		}
		
		// returns the value of the close flag of the specified window
		bool windowShouldClose(LDKWindow* window)
		{
			return window->closeFlag;
		}
		
		void setWindowCloseFlag(LDKWindow* window, bool flag)
		{
			window->closeFlag = flag;	
			if (window->windowCloseCallback)
				window->windowCloseCallback(window);
		}

		// Update the window framebuffer
		void swapWindowBuffer(LDKWindow* window)
		{
			ldk_win32_makeContextCurrent(window);
			bool result = SwapBuffers(window->dc);
			//			if (!result)
			//			{
			//				LogInfo("SwapBuffer error %x", GetLastError());
			//			}
		
			glClearColor(0, 0, 1.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		void showWindow(LDKWindow* window)
		{
			ShowWindow(window->hwnd, SW_SHOW);
		}

		// Get the state of mouse
		const ldk::platform::MouseState* getMouseState()
		{
			return &_platform.mouseState;
		}
		
		// Get the state of keyboard
		const ldk::platform::KeyboardState*	getKeyboardState()
		{
			return &_platform.keyboardState;
		}
		
		// Get the state of a gamepad.
		const ldk::platform::GamepadState* getGamepadState(uint32 gamepadId)
		{
			LDK_ASSERT(( gamepadId >=0 && gamepadId < LDK_MAX_GAMEPADS),"Gamepad id is out of range");
			return &_platform.gamepadState[gamepadId];
		}

		// Updates all windows and OS dependent events
		void pollEvents()
		{
			// clear 'changed' bit from keyboard key state
			for(int i=0; i < LDK_MAX_KBD_KEYS ; i++)
			{
				_platform.keyboardState.key[i] &= ~LDK_KEYSTATE_CHANGED;
			}
		
			// clear 'changed' bit from gamepads buttons state
			for (uint32 id=0; id < LDK_MAX_GAMEPADS; id++)
			{
				// clear 'changed' bit from input key state
				for(uint32 i=0; i < LDK_GAMEPAD_MAX_DIGITAL_BUTTONS ; i++)
				{
					_platform.gamepadState[id].button[i] &= ~LDK_KEYSTATE_CHANGED;
				}
			}
		
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
		
				LDKWindow* window = findWindowByHandle(msg.hwnd);
				switch(msg.message)
				{
					//LDK_ASSERT(window != nullptr, "Could not found a matching window for the current event hwnd");
					case WM_KEYDOWN:
					case WM_KEYUP:
						{
							// bit 30 has previous key state
							// bit 31 has current key state
							// shitty fact: 0 means pressed, 1 means released
							int8 isDown = (msg.lParam & (1 << 31)) == 0;
							int8 wasDown = (msg.lParam & (1 << 30)) != 0;
							int16 vkCode = msg.wParam;
							_platform.keyboardState.key[vkCode] = ((isDown != wasDown) << 1) | isDown;
							continue;
						}
						break;
		
						// Cursor position
					case WM_MOUSEMOVE:
						{
						}
						break;
				}
			}
		}

		ldk::platform::SharedLib* loadSharedLib(char* sharedLibName)
		{
			HMODULE hmodule = LoadLibrary(sharedLibName);
			
			if (!hmodule)
				return nullptr;

			//TODO: Use custom allocation here
			SharedLib* sharedLib = new SharedLib;
			sharedLib->handle = hmodule;
			return sharedLib;
		}
		
		bool unloadSharedLib(ldk::platform::SharedLib* sharedLib)
		{
			if (sharedLib->handle && FreeLibrary(sharedLib->handle))
			{
				sharedLib->handle = NULL;
				delete sharedLib;
				return true;
			}

			return false;
		}
		
		const void* getFunctionFromSharedLib(const ldk::platform::SharedLib* sharedLib, const char* function)
		{
			return GetProcAddress(sharedLib->handle, function);
		}
	} // namespace platform
} // namespace ldk
