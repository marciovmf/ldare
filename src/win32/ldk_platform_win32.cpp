

#ifdef _LDK_WINDOWS_
#define WIN32
#endif // _LDK_WINDOWS_

#include <ldk/ldk.h>
//#include "ldk_win32.cpp"
#include "../ldk_platform.h"
#include "../ldk_gl.h"

// Win32 specifics
#include "tchar.h"
#include <windowsx.h>
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#include <objbase.h>

#define LDK_WINDOW_CLASS "LDK_WINDOW_CLASS"


namespace ldk 
{
	namespace platform 
	{
		static struct LDKWin32
		{
			LDKPlatformErrorFunc 				errorCallback;
			LDKPlatformKeyFunc 					keyCallback;
			LDKPlatformMouseButtonFunc 	mouseButtonCallback;
			LDKPlatformMouseCursorFunc 	mouseCursorCallback;

		} _platform;

		/* platform specific window */
		struct LDKWindow
		{
			HINSTANCE hInstance;
			HWND hwnd;
			HDC dc;
			HGLRC rc;
			bool closeFlag;
			bool fullscreenFlag;
		};

		static HINSTANCE _appInstance;

		LRESULT CALLBACK ldk_win32_windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch(uMsg)
			{
				//TODO: call callbacks here
				default:
					return DefWindowProc(hwnd, uMsg, wParam, lParam);	
					break;
			}
			return TRUE;
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

			//GetWindowRect(gameWindow.hwnd, &gameWindow.windowModeRect);
			//gameWindow.windowModeStyle = GetWindowLong(gameWindow.hwnd, GWL_STYLE);
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

			if(!wglMakeCurrent(gameWindow.dc, gameWindow.rc))
			{
				LogError("Could not make core profile OpenGL context current");
				return false;
			}

			return true;
		}


		/* Error callback function */
		typedef void (* LDKPlatformErrorFunc)(uint32 errorCode, const char* errorMsg);

		/* Keyboard key callback function */
		typedef void (* LDKPlatformKeyFunc) (LDKWindow* window, uint32 key, uint32 action, uint32 modifier);

		/* Mouse button callback function */
		typedef void(* LDKMouseButtonFunc) (LDKWindow* window, uint32 button, uint32 action, uint32 modifier);

		/* Mouse cursor callback function */
		typedef void(* LDKPlatformMouseCursorFunc) (LDKWindow*, double xPos, double yPos);

		// Initialize the platform layer
		uint32 initialize()
		{
			_appInstance = GetModuleHandle(NULL);
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

		// Set the key callback for the given window
		void setKeyCallback(LDKWindow* window, LDKPlatformKeyFunc keyCallback)
		{
			_platform.keyCallback	= keyCallback;
		}

		// set the mouse button callback for the given window
		void setMouseButtonCallback(LDKWindow* window, LDKMouseButtonFunc mouseButtonCallback)
		{
			_platform.mouseButtonCallback	= mouseButtonCallback;
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
				ldk::platform::LDKWindowHint windowHint = (ldk::platform::LDKWindowHint) *pAttribute;


				switch (windowHint)
				{
					case ldk::platform::LDKWindowHint::WIDTH:
						width = *++pAttribute;
						break;
					case ldk::platform::LDKWindowHint::HEIGHT:
						height = *++pAttribute;
						break;
					case ldk::platform::LDKWindowHint::VISIBLE:
						visible = *++pAttribute;
						break;
					case ldk::platform::LDKWindowHint::GL_CONTEXT_VERSION_MAJOR:
						glVersionMajor = *++pAttribute;
						break;
					case ldk::platform::LDKWindowHint::GL_CONTEXT_VERSION_MINOR:
						glVersionMinor = *++pAttribute;
						break;
					case ldk::platform::LDKWindowHint::COLOR_BUFFER_BITS:
						colorBits = *++pAttribute;
						break;
					case ldk::platform::LDKWindowHint::DEPTH_BUFFER_BITS:
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

			if (!ldk_win32_createWindow(window, width, height, _appInstance, (TCHAR*) title))
			{
				LogError("Could not create window");
				return nullptr;
			}

			/* create a new context or share an existing one ? */
			if (share)
			{
				window->rc = share->rc;
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
			DestroyWindow(window->hwnd);
		}

		// returns the value of the close flag of the specified window
		bool windowShouldClose(LDKWindow* window)
		{
			return window->closeFlag;
		}

		// Update the window framebuffer
		void swapWindowBuffer(LDKWindow* window)
		{
			SwapBuffers(window->dc);
		}

		void showWindow(LDKWindow* window)
		{
			ShowWindow(window->hwnd, SW_SHOW);
		}

		// Updates all windows and OS dependent events
		void pollEvents()
		{
			MSG msg;
			// clear 'changed' bit from input key state
//			for(int i=0; i < MAX_KBD_KEYS ; i++)
//			{
//				gameInput.keyboard[i] &= ~KEYSTATE_CHANGED;
//			}

			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

//				switch(msg.message)
//				{
//					case WM_COMMAND:
//						if (LOWORD(msg.wParam) == EDITOR_COMMAND_PLAY)
//						{
//							LogInfo("Play menu selected");
//						}
//						else if (LOWORD(msg.wParam) == EDITOR_COMMAND_STOP)
//						{
//							LogInfo("Stop menu selected");
//						}
//						else if (LOWORD(msg.wParam) == EDITOR_COMMAND_RELOAD)
//						{
//							LogInfo("Reload menu selected");
//							Win32_reloadGameModule(_app.gameModule);
//						}
//
//						// handle keyboard input messages directly
//					case WM_KEYDOWN:
//					case WM_KEYUP:
//						{
//							// bit 30 has previous key state
//							// bit 31 has current key state
//							// shitty fact: 0 means pressed, 1 means released
//							int8 isDown = (msg.lParam & (1 << 31)) == 0;
//							int8 wasDown = (msg.lParam & (1 << 30)) != 0;
//							int16 vkCode = msg.wParam;
//#if _LDK_DEBBUG_
//							if (vkCode == KBD_F12 && isDown)
//							{
//								Win32_toggleFullScreen(_app.window);
//								continue;
//							}
//
//							if (vkCode == KBD_ESCAPE)
//								_app.window.shouldClose = true;
//#endif
//							gameInput.keyboard[vkCode] = ((isDown != wasDown) << 1) | isDown;
//							continue;
//						}
//						break;
//
//						// Cursor position
//					case WM_MOUSEMOVE:
//						{
//							gameInput.cursor.x = GET_X_LPARAM(msg.lParam);
//							gameInput.cursor.y = GET_Y_LPARAM(msg.lParam);
//							continue;
//						}
//						break;
//				}

			}
		}

	}
}
