
#include <ldk/ldk.h>
#include "ldk_platform.h"
#include "ldk_keyboard.cpp"
#include "ldk_gamepad.cpp"

void windowCloseCallback(ldk::platform::LDKWindow* window)
{
	ldk::platform::destroyWindow(window);
}

static void ldkHandleKeyboardInput(ldk::platform::LDKWindow* window, const ldk::KeyboardApi& keyboard)
{
	if (keyboard.getKeyDown(LDK_KEY_ESCAPE))
	{
		ldk::platform::setWindowCloseFlag(window, true);
	}
}

uint32 ldkMain(uint32 argc, char** argv)
{
	ldk::Core core = {};

	if (! ldk::platform::initialize())
	{
		LogError("Error initializing platform layer");
		return LDK_EXIT_FAIL;
	}

	ldk::platform::LDKWindow* window = ldk::platform::createWindow(nullptr, "LDK1", nullptr);

	if (!window)
	{
		LogError("Error creating main window");
		return LDK_EXIT_FAIL;
	}

	ldk::platform::setWindowCloseCallback(window, windowCloseCallback);
	ldk::ldk_keyboard_initApi(&core.keyboard);
	ldk::ldk_gamepad_initApi(&core.gamepad);

	while (!ldk::platform::windowShouldClose(window))
	{
		ldk::platform::pollEvents();
		ldk::ldk_keyboard_update();
		ldk::ldk_gamepad_update();

		ldkHandleKeyboardInput(window, core.keyboard);

		ldk::platform::swapWindowBuffer(window);
	}

	ldk::platform::terminate();

	return LDK_EXIT_SUCCESS;
}

#ifdef _LDK_WINDOWS_
#include <windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//TODO: Handle command line arguments
	return ldkMain(0, nullptr);
}

#ifdef _LDK_DEBUG_
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

