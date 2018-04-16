
#include <ldk/ldk.h>
#include "ldk_platform.h"
#include "ldk_keyboard.cpp"
#include "ldk_gamepad.cpp"
#include "ldk_ini.cpp"
//TODO: use a higher level renderer interface here
//#include "ldk_renderer_gl.cpp"

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

bool loadGameModule(ldk::Game* game, ldk::platform::SharedLib** sharedLib)
{
	*sharedLib = ldk::platform::loadSharedLib(LDK_GAME_MODULE_NAME);

	if (!*sharedLib)
		return false;

	game->init = (ldk::LDK_PFN_GAME_INIT)
		ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_INIT);
	game->start = (ldk::LDK_PFN_GAME_START)
		ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_START);
	game->update = (ldk::LDK_PFN_GAME_UPDATE)
		ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_UPDATE);
	game->stop = (ldk::LDK_PFN_GAME_STOP)
		ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_STOP);

	return game->init && game->start && game->update && game->stop;
}

uint32 ldkMain(uint32 argc, char** argv)
{
	ldk::Core core = {};
	ldk::Game game = {};
	ldk::platform::SharedLib* gameSharedLib;

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

	if (!loadGameModule(&game, &gameSharedLib))
	{
		LogError("Error loading game module");
		return LDK_EXIT_FAIL;
	}

	ldk::platform::setWindowCloseCallback(window, windowCloseCallback);
	ldk::ldk_keyboard_initApi(&core.keyboard);
	ldk::ldk_gamepad_initApi(&core.gamepad);


	ldk::ldk_ini_parseFile((const char8*) "teste.txt");
	game.init(&core);

	game.start();
	while (!ldk::platform::windowShouldClose(window))
	{
		ldk::platform::pollEvents();
		ldk::ldk_keyboard_update();
		ldk::ldk_gamepad_update();

		ldkHandleKeyboardInput(window, core.keyboard);

		game.update(0);

		ldk::platform::swapWindowBuffer(window);
	}

	game.stop();

	if (gameSharedLib)
		ldk::platform::unloadSharedLib(gameSharedLib);

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

