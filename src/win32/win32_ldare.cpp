/**
 * win32_ldare.cpp
 * win32 implementation of ldare engine entrypoint and platform layer
 */
// common ldare headers
#include <ldare/ldare_game.h>
#include <ldare/ldare_editor.h>
// platform independent headers
#include "../ldare_platform.h"
#include "../ldare_memory.h"
#include "../ldare_gl.h"
#include "../ldare_renderer_buffer.h"
#include "../ldare_core.h"
// implementations
#include "win32_platform.cpp"
#include "../ldare_renderer_gl.cpp"
#include "../ldare_renderer_buffer_gl.cpp"
#include "../ldare_asset.cpp"
#include "../ldare_memory.cpp"
// Win32 specifics
#include <windowsx.h>
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#include <objbase.h>
using namespace ldare;
#define GAME_WINDOW_CLASS "LDARE_WINDOW_CLASS"
#define GAME_MODULE_RELOAD_INTERVAL_SECONDS 3.0

namespace ldare
{
	namespace app
	{
		struct Window
		{
			HDC dc;
			HGLRC rc;
			HWND hwnd;
			RECT windowModeRect;
			LONG windowModeStyle;
			bool shouldClose;
			bool isFullScreen;
		};
	}
}
static ldare::app::Window _gameWindow;

static struct EngineDebugService
{
	Material fontMaterial;
  FontAsset* debugFont;
	char fpsText[16];
} _debugService;

struct GameTimer
{
	uint64 lastFrameTime;
	uint64 thisFrameTime;
	float elapsedFrameTime;
	uint32 frameCount;
	float deltaTime;
};

static ldare::GameContext _gameContext;
static ldare::platform::GameModule _gameModuleInfo;

LRESULT CALLBACK LDAREWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{

#if DEBUG
			case WM_ACTIVATE:
				if (wParam != WA_INACTIVE)
				{
					LogInfo("Checking for game changes");
					Win32_reloadGameModule(_gameModuleInfo);
				}
				break;
#endif
		case WM_CLOSE:
			_gameWindow.shouldClose = true;	
			break;

		case WM_MOVE:
			{			
				// update window mode rect
				if ( !_gameWindow.isFullScreen)
				{
					GetWindowRect(hwnd, &_gameWindow.windowModeRect);
				}
			}
			break;
		case WM_SIZE:
			{
				RECT rect;
				GetClientRect(hwnd, &rect);
				setViewportAspectRatio(rect.right, rect.bottom, _gameContext.Resolution.width, _gameContext.Resolution.height);
				// update window mode rect
				if (wParam == SIZE_RESTORED && !_gameWindow.isFullScreen)
				{
					GetWindowRect(hwnd, &rect);
					_gameWindow.windowModeRect.right = (uint32)LOWORD(lParam);
					_gameWindow.windowModeRect.bottom = (uint32)HIWORD(lParam);
				}
				break;
			}

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
	windowClass.lpfnWndProc = LDAREWindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	windowClass.lpszClassName = className;
	return RegisterClass(&windowClass) != 0;
}

static void Win32_toggleFullScreen(ldare::app::Window& gameWindow)
{
	LONG currentStyle =  GetWindowLong(gameWindow.hwnd, GWL_STYLE);
	LONG newStyle = 0;
	RECT newRect;
	if (gameWindow.isFullScreen)
	{
		LogInfo("leaving full screen");
		newStyle = gameWindow.windowModeStyle;
		newRect = gameWindow.windowModeRect;
		gameWindow.isFullScreen = false;
	}
	else
	{
		LogInfo("Entering full screen");
		newStyle = WS_POPUP;
		GetClientRect(GetDesktopWindow(), &newRect);
		gameWindow.isFullScreen = true;
	}

	SetWindowLong(gameWindow.hwnd, GWL_STYLE, newStyle);
	SetWindowPos(gameWindow.hwnd, HWND_TOP, newRect.left, newRect.top, newRect.right, newRect.bottom, SWP_SHOWWINDOW);
}

static bool Win32_CreateGameWindow(
		ldare::app::Window& gameWindow, int width, int height, HINSTANCE hInstance, TCHAR* title)
{
	gameWindow.hwnd = CreateWindowEx(NULL, 
			TEXT(GAME_WINDOW_CLASS),
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

	GetWindowRect(gameWindow.hwnd, &gameWindow.windowModeRect);
	gameWindow.windowModeStyle = GetWindowLong(gameWindow.hwnd, GWL_STYLE);
	if (!gameWindow.hwnd)
		return false;

	gameWindow.dc = GetDC(gameWindow.hwnd);

	return gameWindow.dc != NULL;
}

static bool Win32_InitOpenGL(ldare::app::Window& gameWindow, HINSTANCE hInstance, int major, int minor)
{
	ldare::app::Window dummyWindow = {};
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
	(name = (type) ldare::platform::Win32_getGlFunctionPointer((const char*)#name))
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
#ifdef DEBUG
		WGL_CONTEXT_DEBUG_BIT_ARB |
#endif
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

static inline void Win32_processPendingMessages(HWND hwnd, ldare::Input& gameInput)
{
	MSG msg;
	// clear 'changed' bit from input key state
	for(int i=0; i < MAX_KBD_KEYS ; i++)
	{
		gameInput.keyboard[i] &= ~KEYSTATE_CHANGED;
	}

	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		switch(msg.message)
		{
			case WM_COMMAND:
				if (LOWORD(msg.wParam) == EDITOR_COMMAND_PLAY)
				{
					LogInfo("Play menu selected");
				}
				else if (LOWORD(msg.wParam) == EDITOR_COMMAND_STOP)
				{
					LogInfo("Stop menu selected");
				}
				else if (LOWORD(msg.wParam) == EDITOR_COMMAND_RELOAD)
				{
					LogInfo("Reload menu selected");
					Win32_reloadGameModule(_gameModuleInfo);
				}

		// handle keyboard input messages directly
			case WM_KEYDOWN:
			case WM_KEYUP:
				{
					// bit 30 has previous key state
					// bit 31 has current key state
					// shitty fact: 0 means pressed, 1 means released
					int8 isDown = (msg.lParam & (1 << 31)) == 0;
					int8 wasDown = (msg.lParam & (1 << 30)) != 0;
					int16 vkCode = msg.wParam;
#if DEBUG
				if (vkCode == KBD_F12 && isDown)
				{
					Win32_toggleFullScreen(_gameWindow);
					continue;
				}

				if (vkCode == KBD_ESCAPE)
						_gameWindow.shouldClose = true;
#endif
					gameInput.keyboard[vkCode] = ((isDown != wasDown) << 1) | isDown;
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

static inline void Win32_processGamepadInput(ldare::Input& gameInput)
{
	// clear 'changed' bit from input key state
	for(int gamepadIndex=0; gamepadIndex < MAX_GAMEPADS ; gamepadIndex++)
	{
		for(int i=0; i < GAMEPAD_MAX_DIGITAL_BUTTONS ; i++)
		{
			gameInput.gamepad[gamepadIndex].button[i] &= ~KEYSTATE_CHANGED;
		}
	}

	// get gamepad input
	for(int16 gamepadIndex = 0; gamepadIndex < MAX_GAMEPADS; gamepadIndex++)
	{
		ldare::platform::XINPUT_STATE gamepadState;
		Gamepad& gamepad = gameInput.gamepad[gamepadIndex];

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
		wasDown = gamepad.button[btn] & KEYSTATE_PRESSED;\
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

static void initGameApi(ldare::GameApi& gameApi)
{
	// Initialize the API exposed to the game
	initSpriteBatch();
	gameApi.spriteBatch.begin = ldare::begin;
	gameApi.spriteBatch.submit = ldare::submit;
	gameApi.spriteBatch.end = ldare::end;
	gameApi.spriteBatch.flush = ldare::flush;
	gameApi.spriteBatch.loadShader = ldare::loadShader;

	// Text api
	initSpriteBatch();
	gameApi.text.begin = ldare::beginText;
	gameApi.text.drawText = ldare::drawText;
	gameApi.text.end = ldare::endText;
	gameApi.text.flush = ldare::flushText;

	// init asset api
	gameApi.asset.loadMaterial = ldare::loadMaterial;
	gameApi.asset.loadAudio = ldare::loadAudio;
	gameApi.asset.loadFont = ldare::loadFont;
	gameApi.audio.playAudio = ldare::playAudio;
}

void addEditorMenu(ldare::app::Window& window)
{
	HMENU menuBar = CreateMenu();
	// Game menu
	HMENU gameMenu = CreatePopupMenu();
	AppendMenuA(menuBar, MF_ENABLED | MF_POPUP, (UINT_PTR)gameMenu, "&Game");
	AppendMenu(gameMenu, MF_ENABLED | MF_STRING, EDITOR_COMMAND_RELOAD, "&Reload");
	AppendMenu(gameMenu, MF_ENABLED | MF_STRING, EDITOR_COMMAND_PLAY, "&Play");
	AppendMenu(gameMenu, MF_ENABLED | MF_STRING, EDITOR_COMMAND_STOP, "&Stop");

	SetMenu(window.hwnd, menuBar);
}

//---------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------
#if 0
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LogInfo("Initializing");
	platform::Win32_setCurrentDirectory();

	ldare::Input gameInput = {};
	GameApi gameApi = {};
	_gameModuleInfo = {};
	_gameModuleInfo.moduleFileName = "ldare_game.dll";

	// Load the game module
	if(!platform::loadGameModule(_gameModuleInfo))
	{
		return FALSE;
	}

	// Initialize the game settings
	_gameContext = _gameModuleInfo.init();

	// Reserve memory for the game
	void* gameMemory = platform::memoryAlloc(_gameContext.gameMemorySize);

	if ( !Win32_RegisterGameWindowClass(hInstance,TEXT(GAME_WINDOW_CLASS)) )
	{
		LogError("Could not register window class");
	}

	if (!Win32_CreateGameWindow(_gameWindow, _gameContext.windowWidth,
				_gameContext.windowHeight, hInstance, TEXT("lDare Engine") ))
	{
		LogError("Could not create window");
	}

	if (! Win32_InitOpenGL(_gameWindow, hInstance, 3, 3))
	{
		LogError("Could not initialize OpenGL for game window" );
	}

	// fullscreen
	if (_gameContext.fullScreen)
	{
		Win32_toggleFullScreen(_gameWindow);
	}

	CoInitialize(NULL);
	platform::Win32_initXInput();
	platform::Win32_initXAudio();
	initGameApi(gameApi);

	addEditorMenu(_gameWindow);

	// start the game
	_gameModuleInfo.start(gameMemory, gameApi);

	if (_gameContext.Resolution.width ==0 ) _gameContext.Resolution.width = _gameContext.windowWidth;
	if (_gameContext.Resolution.height ==0 ) _gameContext.Resolution.height = _gameContext.windowHeight;

	ShowWindow(_gameWindow.hwnd, SW_SHOW);

	platform::Win32_initTimer();
	GameTimer gameTimer = {};

#ifdef DEBUG
	_debugService.fontMaterial = gameApi.asset.loadMaterial(
			(const char*)"./assets/font.vert", 
			(const char*) "./assets/font.frag", 
			(const char*)"./assets/Liberation Mono.bmp");

		gameApi.asset.loadFont(
			(const char*)"./assets/Liberation Mono.font", &_debugService.debugFont);

#endif

	while (!_gameWindow.shouldClose)
	{
		gameTimer.lastFrameTime = gameTimer.thisFrameTime;
		gameTimer.thisFrameTime = platform::getTicks();


#if DEBUG
		// Check for new game DLL every 180 frames
		if (_gameModuleInfo.timeSinceLastReload >= GAME_MODULE_RELOAD_INTERVAL_SECONDS)
		{
			// if game reloaded, run start again
			if (Win32_reloadGameModule(_gameModuleInfo))
			{
				LogInfo("Game module reloaded");
				_gameModuleInfo.timeSinceLastReload = 0;
				_gameModuleInfo.start(gameMemory, gameApi);
			}
		}
		else
		{
			_gameModuleInfo.timeSinceLastReload += gameTimer.deltaTime;
		}
#endif

		Win32_processPendingMessages(_gameWindow.hwnd, gameInput);
		Win32_processGamepadInput(gameInput);

		//Update the game
		updateRenderer(gameTimer.deltaTime);
		_gameModuleInfo.update(gameTimer.deltaTime, gameInput, gameApi);

#if DEBUG
		gameApi.text.begin(*_debugService.debugFont, _debugService.fontMaterial);
			gameApi.text.drawText(Vec3{5, _gameContext.windowHeight - 30, 2}, 1.0f, Vec4{0.0f, 0.0f, 0.0f, 1.0f}, _debugService.fpsText);
		gameApi.text.end();
#endif

		SwapBuffers(_gameWindow.dc);

		// get deltaTime
		gameTimer.deltaTime = platform::getTimeBetweenTicks(gameTimer.lastFrameTime, gameTimer.thisFrameTime);

		gameTimer.frameCount++;
		gameTimer.elapsedFrameTime += gameTimer.deltaTime;

#if DEBUG
		// count frames per second
		if (gameTimer.elapsedFrameTime>1)
		{
			gameTimer.elapsedFrameTime -=1;
			sprintf(_debugService.fpsText, "FPS: %d", gameTimer.frameCount);
			gameTimer.frameCount=0;
		}

#endif
	}

	_gameModuleInfo.stop();
	LogInfo("Finished");
	CoUninitialize();
	return 0;
}
#endif

namespace ldare
{
	namespace app
	{
		static struct Application
		{
			HINSTANCE hInstance;
			Window window;
		} _app;

		bool init(uint32 renderApi)
		{
			if (renderApi != OPENGL_3_2)
			{
				LogError("Unsupported render api");
				return false;
			}

			platform::Win32_setCurrentDirectory();
			_app.hInstance = GetModuleHandleA(NULL);
			_app.window = {};

			CoInitialize(NULL);

			// Initialize windows specific dependencies
			platform::Win32_initXInput();
			platform::Win32_initXAudio();
			platform::Win32_initTimer();

			return true;
		}

		Window* createWindow(uint32 width, uint32 height, const char* title)
		{
			if ( !Win32_RegisterGameWindowClass(_app.hInstance,TEXT(GAME_WINDOW_CLASS)) )
			{
				LogError("Could not register game window class");
				return nullptr;
			}

			if (!Win32_CreateGameWindow(_app.window, width, height, _app.hInstance, (TCHAR*)title))
			{
				LogError("Could not create window");
				return nullptr;
			}
			
			Win32_InitOpenGL(_app.window, _app.hInstance, 3, 2);
			ShowWindow(_app.window.hwnd, SW_SHOW);
			return &_app.window;
		}

		inline void swapBuffer(Window& window)
		{
			SwapBuffers(window.dc);
		}

		inline bool shouldClose(Window& window)
		{
			return window.shouldClose;
		}

		void pollEvents(Window& window, ldare::Input& gameInput)
		{
			Win32_processPendingMessages(_gameWindow.hwnd, gameInput);
			Win32_processGamepadInput(gameInput);
		}

		void terminate(Application& context)
		{
			//TODO: do window and application cleanup here
		}
	}
}

#ifdef DEBUG
using namespace ldare::app;

int _tmain(int argc, _TCHAR** argv)
{
	//return WinMain(GetModuleHandle(NULL), NULL, NULL,SW_SHOW);

	Window* window;
	ldare::Input gameInput = {};
	GameTimer gameTimer = {};
	GameApi gameApi = {};
	_gameModuleInfo = {};
	_gameModuleInfo.moduleFileName = "ldare_game.dll";

	// initialize
	if (!init(OPENGL_3_2))
	{
		LogError("Error initializing LDARE engine");
		return -1;
	}

	// Load the game module
	if(!platform::loadGameModule(_gameModuleInfo))
	{
		return FALSE;
	}

	// Create game window
	if( !(window = createWindow(800, 600, "ldare engine")))
	{
		return -1;
	}

	// Initialize the game settings
	_gameContext = _gameModuleInfo.init();

	// Reserve memory for the game
	void* gameMemory = platform::memoryAlloc(_gameContext.gameMemorySize);

	initGameApi(gameApi);
	addEditorMenu(_gameWindow);

	// start the game
	_gameModuleInfo.start(gameMemory, gameApi);

	if (_gameContext.Resolution.width ==0 ) _gameContext.Resolution.width = _gameContext.windowWidth;
	if (_gameContext.Resolution.height ==0 ) _gameContext.Resolution.height = _gameContext.windowHeight;

#ifdef DEBUG
	_debugService.fontMaterial = gameApi.asset.loadMaterial(
			(const char*)"./assets/font.vert", 
			(const char*) "./assets/font.frag", 
			(const char*)"./assets/Liberation Mono.bmp");

		gameApi.asset.loadFont(
			(const char*)"./assets/Liberation Mono.font", &_debugService.debugFont);

#endif

	while (!shouldClose(*window))
	{
		gameTimer.lastFrameTime = gameTimer.thisFrameTime;
		gameTimer.thisFrameTime = platform::getTicks();

#if DEBUG
		// Check for new game DLL every 180 frames
		if (_gameModuleInfo.timeSinceLastReload >= GAME_MODULE_RELOAD_INTERVAL_SECONDS)
		{
			// if game reloaded, run start again
			if (Win32_reloadGameModule(_gameModuleInfo)) //TODO: make this platform independent
			{
				LogInfo("Game module reloaded");
				_gameModuleInfo.timeSinceLastReload = 0;
				_gameModuleInfo.start(gameMemory, gameApi);
			}
		}
		else
		{
			_gameModuleInfo.timeSinceLastReload += gameTimer.deltaTime;
		}
#endif

		pollEvents(*window, gameInput);

		//Update the game
		updateRenderer(gameTimer.deltaTime);
		_gameModuleInfo.update(gameTimer.deltaTime, gameInput, gameApi);

#if DEBUG
		gameApi.text.begin(*_debugService.debugFont, _debugService.fontMaterial);
			gameApi.text.drawText(Vec3{5, _gameContext.windowHeight - 30, 2}, 1.0f, Vec4{0.0f, 0.0f, 0.0f, 1.0f}, _debugService.fpsText);
		gameApi.text.end();
#endif

		SwapBuffers(_gameWindow.dc);

		// get deltaTime
		gameTimer.deltaTime = platform::getTimeBetweenTicks(gameTimer.lastFrameTime, gameTimer.thisFrameTime);

		gameTimer.frameCount++;
		gameTimer.elapsedFrameTime += gameTimer.deltaTime;

#if DEBUG
		// count frames per second
		if (gameTimer.elapsedFrameTime>1)
		{
			gameTimer.elapsedFrameTime -=1;
			sprintf(_debugService.fpsText, "FPS: %d", gameTimer.frameCount);
			gameTimer.frameCount=0;
		}

#endif
	}

	_gameModuleInfo.stop();
	LogInfo("Finished");
	CoUninitialize();
	return 0;

}
#endif //DEBUG

