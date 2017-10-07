/**
 * win32_ldare.cpp
 * win32 implementation of ldare engine entrypoint and platform layer
 */
// common ldare headers
#include <ldare/ldare_game.h>
// platform independent headers
#include "../ldare_platform.h"
#include "../ldare_memory.h"
#include "../ldare_gl.h"
// implementations
#include "win32_platform.cpp"
#include "../ldare_renderer_gl.cpp"
#include "../ldare_asset.cpp"
#include "../ldare_memory.cpp"
// Win32 specifics
#include <windowsx.h>
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
using namespace ldare;
#define GAME_WINDOW_CLASS "LDARE_WINDOW_CLASS"

static struct Win32_GameWindow
{
	HDC dc;
	HGLRC rc;
	HWND hwnd;
	RECT windowModeRect;
	LONG windowModeStyle;
	bool shouldClose;
} _gameWindow;

struct Win32_GameModuleInfo
{
	const char* moduleFileName;
	HMODULE hGameModule;
	FILETIME lastWriteTime;
	gameInitFunc *init;
	gameStartFunc *start;
	gameUpdateFunc *update;
	gameStopFunc *stop;
	int32 framesSinceLastReload;
};

static FILETIME Win32_getFileWriteTime(const char* fileName)
{
	FILETIME writeTime;
	HANDLE handle = CreateFileA(fileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	GetFileTime(handle, 0, 0, &writeTime);
	CloseHandle(handle);
	return writeTime;
}

//---------------------------------------------------------------------------
// Loads the Game dll.
// Returns: true if successfully loads the game dll 
//	AND fetches the Update and Start function pointers
// Globals: gameModuleInfo
//---------------------------------------------------------------------------
static inline bool Win32_loadGameModule(Win32_GameModuleInfo& gameModuleInfo)
{
	//TODO: marcio, make sure executable directory is current directory.
	const char* dllFileName = gameModuleInfo.moduleFileName;

#if DEBUG
	// load a copy of the dll, so the original can be recompiled
	const char* dllCopyFileName = "ldare_game_copy.dll";
	if (!CopyFileA(dllFileName, dllCopyFileName, false))
	{
		LogError("Error copying game dll\n");
		return false;
	}
	dllFileName = dllCopyFileName;
	gameModuleInfo.lastWriteTime = Win32_getFileWriteTime(gameModuleInfo.moduleFileName);
#endif

	if ((gameModuleInfo.hGameModule = LoadLibraryA(dllFileName)))
	{
		GetFileTime(gameModuleInfo.hGameModule, 0, 0, &gameModuleInfo.lastWriteTime);
		gameModuleInfo.init = (gameInitFunc*)GetProcAddress(gameModuleInfo.hGameModule, "gameInit");
		gameModuleInfo.start = (gameStartFunc*)GetProcAddress(gameModuleInfo.hGameModule, "gameStart");
		gameModuleInfo.update = (gameUpdateFunc*)GetProcAddress(gameModuleInfo.hGameModule, "gameUpdate");
		gameModuleInfo.stop = (gameStopFunc*)GetProcAddress(gameModuleInfo.hGameModule, "gameStop");
	}
	else
	{
		LogError("Error loading game module\n");
		return false;
	}

	if (!(gameModuleInfo.init && gameModuleInfo.start && gameModuleInfo.update && gameModuleInfo.stop))
		return false;

	return true;
}



//---------------------------------------------------------------------------
// Checks if there is a newer game dll and loads it if it does.
// Returns: true if the module was loaded or reloaded
// Globals: gameModuleInfo
//---------------------------------------------------------------------------
static inline bool Win32_reloadGameModule(Win32_GameModuleInfo& gameModuleInfo)
{
	bool reloaded = false;

	// Is it already loaded ?
	if (gameModuleInfo.hGameModule != 0)
	{
		FILETIME writeTime = Win32_getFileWriteTime(gameModuleInfo.moduleFileName);
		// Is there a newer version ?
		if (CompareFileTime(&writeTime, &gameModuleInfo.lastWriteTime) > 0)
		{
			FreeLibrary(gameModuleInfo.hGameModule);
			gameModuleInfo.lastWriteTime = writeTime;
			reloaded = true;
		}
	}
	else
	{
		reloaded = true;
		Win32_loadGameModule(gameModuleInfo);
	}

	return reloaded;
}

LRESULT CALLBACK Win32_GameWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CLOSE:
			_gameWindow.shouldClose = true;	
			break;
		case WM_SIZE:
			RECT windowRect;
			GetClientRect(_gameWindow.hwnd,&windowRect);
			//TODO: remove GL calls from here!
			glViewport(0,0, windowRect.right, windowRect.bottom);
			break;

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);	
			break;
	}

	return TRUE;
}

static bool Win32_RegisterGameWindowClass(HINSTANCE hInstance, TCHAR* className)
{
	WNDCLASS windowClass = {};
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = Win32_GameWindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	windowClass.lpszClassName = className;
	return RegisterClass(&windowClass) != 0;
}

static void Win32_toggleFullScreen(Win32_GameWindow& gameWindow)
{
	HWND desktop = GetDesktopWindow();
	RECT rect;
	GetClientRect(desktop, &rect);
	SetWindowLong(gameWindow.hwnd, GWL_STYLE, WS_POPUP);
	SetWindowPos(gameWindow.hwnd, HWND_TOP, 0, 0, rect.right, rect.bottom, SWP_FRAMECHANGED);
}

static bool Win32_CreateGameWindow(
		Win32_GameWindow& gameWindow, int width, int height, HINSTANCE hInstance, TCHAR* title)
{
	gameWindow.hwnd = CreateWindowEx(NULL, 
			TEXT(GAME_WINDOW_CLASS),
			title,
			WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_BORDER,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			NULL,
			NULL,
			hInstance,
			NULL);

	GetClientRect(gameWindow.hwnd, &gameWindow.windowModeRect);
	gameWindow.windowModeStyle = GetWindowLong(gameWindow.hwnd, GWL_STYLE);
	if (!gameWindow.hwnd)
		return false;

	gameWindow.dc = GetDC(gameWindow.hwnd);
	return gameWindow.dc != NULL;
}


static bool Win32_InitOpenGL(Win32_GameWindow* gameWindow, HINSTANCE hInstance, int major, int minor)
{
	Win32_GameWindow dummyWindow = {};
	if (! Win32_CreateGameWindow(dummyWindow,0,0,hInstance,TEXT("")) )
	{
		LogError("Could not create a dummy window for openGl initialization");
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER ;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
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
	(name = (type) platform::getGlFunctionPointer((const char*)#name))
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
			gameWindow->dc,
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

	if (! SetPixelFormat(gameWindow->dc, pfId, &pfd))
	{
		LogError("Could not set pixel format for OpenGL context creation");
		return false;
	}

	const int contextAttribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB,
#ifdef DEBUG
		WGL_CONTEXT_DEBUG_BIT_ARB |
#endif
			WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	gameWindow->rc = wglCreateContextAttribsARB(
			gameWindow->dc,
			0,
			contextAttribs);

	if (!gameWindow->rc)
	{
		LogError("Could not create a core profile OpenGL context");
		return false;
	}

	if(!wglMakeCurrent(gameWindow->dc, gameWindow->rc))
	{
		LogError("Could not make core profile OpenGL context current");
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------
// processes Windows Keyboard messages
//---------------------------------------------------------------------------
static inline void Win32_ProcessKeyboardMessage(ldare::KeyState& keyState, int8 lastState,int8 state)
{
	keyState.state = state;
	keyState.thisFrame += state != lastState || keyState.thisFrame;
}

static void initGameApi(ldare::GameApi& gameApi)
{
	// Initialize the API exposed to the game
	initSpriteBatch();
	gameApi.spriteBatch.begin = ldare::begin;
	gameApi.spriteBatch.submit = ldare::submit;
	gameApi.spriteBatch.end = ldare::end;
	gameApi.spriteBatch.flush = ldare::flush;
	gameApi.spriteBatch.loadShader = ldare::loadShader;

	// init asset api
	gameApi.asset.loadMaterial = ldare::loadMaterial;
}


static inline void processPendingMessages(HWND hwnd, ldare::Input& gameInput)
{
	MSG msg;

	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		// handle keyboard input messages directly
		switch(msg.message)
		{
			case WM_ACTIVATE:
				LogInfo("wparam is %d", LOWORD(msg.wParam));
				break;
			case WM_KEYDOWN:
			case WM_KEYUP:
				{
					// bit 30 has previous key state
					// bit 31 has current key state
					// shitty fact: 0 means pressed, 1 means released
					int8 isDown = (msg.lParam & (1 << 31)) == 0;
					int8 wasDown = (msg.lParam & (1 << 30)) != 0;
					int16 vkCode = msg.wParam;
					ldare::KeyState& currentState = gameInput.keyboard[vkCode];

					Win32_ProcessKeyboardMessage(gameInput.keyboard[vkCode], wasDown, isDown);
					continue;
				}
				break;

				// Cursor position
			case WM_MOUSEMOVE:
				{
					gameInput.cursor.x = GET_X_LPARAM(msg.lParam);
					gameInput.cursor.y = GET_Y_LPARAM(msg.lParam);
					continue;
				}
				break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg) ;
	}
}

//---------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LogInfo("Initializing");
	ldare::Input gameInput = {};
	Win32_GameModuleInfo gameModuleInfo = {};
	gameModuleInfo.moduleFileName = "ldare_game.dll";
	GameApi gameApi = {};

	// Load the game module
	if(!Win32_reloadGameModule(gameModuleInfo))
	{
		return FALSE;
	}

	// Initialize the game settings
	ldare::GameContext gameContext = gameModuleInfo.init();
  
	// Reserve memory for the game
	void* gameMemory = platform::memoryAlloc(gameContext.gameMemorySize);

	if ( !Win32_RegisterGameWindowClass(hInstance,TEXT(GAME_WINDOW_CLASS)) )
	{
		LogError("Could not register window class");
	}

	if (!Win32_CreateGameWindow(_gameWindow, gameContext.windowWidth,
				gameContext.windowHeight, hInstance, TEXT("lDare Engine") ))
	{
		LogError("Could not create window");
	}

	if (! Win32_InitOpenGL(&_gameWindow, hInstance, 3, 3))
	{
		LogError("Could not initialize OpenGL for game window" );
	}

	// fullscreen
	if (gameContext.fullScreen)
	{
		Win32_toggleFullScreen(_gameWindow);
	}

	initGameApi(gameApi);

	// start the game
	gameModuleInfo.start(gameMemory, gameApi);
	
	if (gameContext.Resolution.width ==0 ) gameContext.Resolution.width = gameContext.windowWidth;
  if (gameContext.Resolution.height ==0 ) gameContext.Resolution.height = gameContext.windowHeight;

	//TODO: marcio, find somewhere else to set clear color that can happen along the game loop
	//TODO: marcio, remove opengl calls from here and move it to renderer layer
	ShowWindow(_gameWindow.hwnd, SW_SHOW);

	while (!_gameWindow.shouldClose)
	{
#if DEBUG
		// Check for new game DLL every 180 frames
		if (gameModuleInfo.framesSinceLastReload >= 180)
		{
			// if game reloaded, run start again
			if (Win32_reloadGameModule(gameModuleInfo))
			{
				LogInfo("Reloading game module");
				gameModuleInfo.start(gameMemory, gameApi);
			}

			gameModuleInfo.framesSinceLastReload = 0;
		}
		else
		{
			gameModuleInfo.framesSinceLastReload++;
		}
#endif

		// clear 'this frame' flags from input key state
		for(int i=0; i < MAX_GAME_KBD_KEYS ; i++)
		{
			gameInput.keyboard[i].thisFrame = 0;
		}

		processPendingMessages(_gameWindow.hwnd, gameInput);

		//Update the game
		gameModuleInfo.update(gameInput, gameApi);
		SwapBuffers(_gameWindow.dc);
	}

	gameModuleInfo.stop();
	LogInfo("Finished");
	return 0;
}

#ifdef DEBUG
int _tmain(int argc, _TCHAR** argv)
{
	return WinMain(GetModuleHandle(NULL), NULL, NULL,SW_SHOW);
}
#endif //DEBUG

