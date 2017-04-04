#include <windows.h>
#include <tchar.h>

#ifdef DEBUG
#include <iostream>

#ifdef UNICODE
#define OUTSTREAM std::cout
#define ERRSTREAM std::cerr
#else
#define OUTSTREAM std::cout
#define ERRSTREAM std::cerr
#endif // UNICODE

#define LogMsg(prefix, msg, stdstream) stdstream << prefix << " " << msg << "\n" << __FILE__ << ":" << __LINE__ <<  std::endl
#define LogInfo(msg) LogMsg("[INFO]", msg, OUTSTREAM)
#define LogWarning(msg) LogMsg("[WARNING]", msg, OUTSTREAM)
#define LogError(msg) LogMsg("[ERROR]", msg, OUTSTREAM)
#else
#define LogMsg(prefix, msg, stdstream)
#define LogError(msg) 
#define LogWarning(msg) 
#define LogError(msg) 

#endif // DEBUG

#define GAME_WINDOW_CLASS "LDARE_WINDOW_CLASS"

struct GameWindow
{
	HDC dc;
	HWND hwnd;
	bool shouldClose;
};

static GameWindow _gameWindow;

LRESULT CALLBACK GameWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

static bool RegisterGameWindowClass(HINSTANCE hInstance, TCHAR* className)
{
	WNDCLASS windowClass = {};
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = GameWindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	windowClass.lpszClassName = className;
	return RegisterClass(&windowClass) != 0;
}

static bool CreateGameWindow(
		GameWindow* gameWindow, int width, int height, HINSTANCE hInstance, TCHAR* title)
{
		 gameWindow->hwnd = CreateWindow(TEXT(GAME_WINDOW_CLASS),
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

		gameWindow->dc = GetDC(gameWindow->hwnd);
		return gameWindow->dc != NULL;

}

int CALLBACK WinMain(
		HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LogInfo("Initializing");

	if ( !RegisterGameWindowClass(hInstance,TEXT(GAME_WINDOW_CLASS)) )
	{
		LogError("Could not register window class");
	}

	if (!CreateGameWindow(&_gameWindow, 800, 600, hInstance, TEXT("lDare Engine") ))
	{
		LogError("Could not create window");
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

	LogInfo("Finished");
	return 0;
}

#ifdef DEBUG
int _tmain(int argc, _TCHAR** argv)
{
	return WinMain(GetModuleHandle(NULL), NULL, NULL,SW_SHOW);
}
#endif //DEBUG


