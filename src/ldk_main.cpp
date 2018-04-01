
#include <ldk/ldk.h>
#include "ldk_platform.h"

void keyboardCallback(ldk::platform::LDKWindow* window, uint32 key, uint32 action, uint32 modifier)
{
	if (key == LDK_KEY_ESCAPE)
	{
		if (action == LDK_KEY_PRESS)
		{
			ldk::platform::setWindowCloseFlag(window, true);
		}
	}
}

void windowCloseCallback(ldk::platform::LDKWindow* window)
{
	ldk::platform::destroyWindow(window);
}

uint32 ldkMain(uint32 argc, char** argv)
{
	if (! ldk::platform::initialize())
	{
		LogError("Error initializing platform layer");
		return LDK_EXIT_FAIL;
	}

	ldk::platform::LDKWindow* window = ldk::platform::createWindow(nullptr, "LDK1", nullptr);
	ldk::platform::LDKWindow* window2 = ldk::platform::createWindow(nullptr, "LDK2", window);

	if (!window || !window2)
	{

		LogError("Error creating main window");
		return LDK_EXIT_FAIL;
	}

	/* Set callbacks for window 1*/
	ldk::platform::setKeyCallback(window, keyboardCallback);
	ldk::platform::setWindowCloseCallback(window, windowCloseCallback);

	/* Set callbacks for window 2*/
	ldk::platform::setKeyCallback(window2, keyboardCallback);
	ldk::platform::setWindowCloseCallback(window2, windowCloseCallback);

	while (!ldk::platform::windowShouldClose(window))
	{
		ldk::platform::pollEvents();
		ldk::platform::swapWindowBuffer(window);
		ldk::platform::swapWindowBuffer(window2);
	}

	ldk::platform::terminate();

	return LDK_EXIT_SUCCESS;
}

#if _LDK_WINDOWS_
#include <windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//TODO: Handle command line arguments
	return ldkMain(0, nullptr);
}

#if _LDK_DEBUG_
#include <tchar.h>
int _tmain(int argc, _TCHAR** argv)
{
	//TODO: parse command line here and pass it to winmain
	return WinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOW);
}
#endif // _LDK_DEBUG_

#else // _LDK_WINDOWS_
int main(int argc, char** argv)
{
	return ldkMain(argc, argv);
}
#endif

