#include <windows.h>
#include <tchar.h>

#define GAME_WINDOW_CLASS "LDARE_WINDOW_CLASS"

//TODO: Add LogInfo() and LogWarning() macros
#ifdef DEBUG
#include <iostream>
#define logError(msg) std::cerr << msg << std::endl;
#else
#define logError(msg) 
#endif

struct GameWindow
{
	HDC dc;
	HWND hwnd;
	bool shouldClose;
};

static GameWindow _gameWindow;


LRESULT CALLBACK GameWindowProc(
  HWND   hwnd,
  UINT   uMsg,
  WPARAM wParam,
  LPARAM lParam
)
{
	switch(uMsg)
	{
		case WM_CLOSE:
		_gameWindow.shouldClose = true;	
		break;

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);	
		break;
	}

	return TRUE;

}

static bool RegisterGameWindowClass(HINSTANCE hInstance)
{
	WNDCLASS windowClass = {};
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = GameWindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	windowClass.lpszClassName = GAME_WINDOW_CLASS;

	return RegisterClass(&windowClass) != 0;
}

static bool CreateGameWindow(GameWindow* gameWindow, int width, int height, HINSTANCE hInstance, const char* title)
{
		 gameWindow->hwnd = CreateWindow(GAME_WINDOW_CLASS,
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

		if (!gameWindow->hwnd)
			return false;
		//TODO: Logo error here

		gameWindow->dc = GetDC(gameWindow->hwnd);
		//TODO: Logo error here if DC is NULL
		return gameWindow->dc != NULL;

}

int CALLBACK WinMain(
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR     lpCmdLine,
		int       nCmdShow
		)
{
	if ( !RegisterGameWindowClass(hInstance) )
	{
		logError("Could not register window class ");
	}

	if (!CreateGameWindow(&_gameWindow, 800, 600, hInstance, "lDare Engine" ))
	{
		logError("Could not create window");
	}

	ShowWindow(_gameWindow.hwnd, SW_SHOW);

	while (!_gameWindow.shouldClose)
	{
		MSG msg;
		while (PeekMessage(&msg, _gameWindow.hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}
//TODO: remove IOSTREAM from here
//TODO: Replace raw std::cout for logError
#ifdef DEBUG
#include <iostream>
int _tmain(int argc, _TCHAR** argv)
{
	std::cout << "Initializing ldare ..." << std::endl;
	return WinMain(GetModuleHandle(NULL), NULL, NULL,SW_SHOW);
}
#endif
