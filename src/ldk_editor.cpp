
#include <ldk/ldk.h>
#include "ldk_platform.h"
#include "ldk_memory.h"

#define LDK_DEFAULT_GAME_WINDOW_TITLE "LDK Window"
#define LDK_DEFAULT_CONFIG_FILE "ldk.cfg"

struct GameConfig
{
	int32 width;
	int32 height;
	bool fullscreen;
	float aspect;
	char* title;
} defaultConfig;

static int64 lastGameDllTime = 0;

void windowCloseCallback(ldk::platform::LDKWindow* window)
{
	ldk::platform::destroyWindow(window);
}

void windowResizeCallback(ldk::platform::LDKWindow* window, int32 width, int32 height)
{
	// Recalculate projection matrix here.
	//ldk::render::setViewportAspectRatio(width, height, defaultConfig.width, defaultConfig.height);
	return;
}

static void ldkHandleKeyboardInput(ldk::platform::LDKWindow* window)
{
	if (ldk::input::isKeyDown(LDK_KEY_ESCAPE))
	{
		ldk::platform::setWindowCloseFlag(window, true);
	}
	
	if (ldk::input::isKeyDown(LDK_KEY_F12))
	{
		ldk::platform::toggleFullScreen(window, !ldk::platform::isFullScreen(window));
	}
}

bool loadGameModule(char* gameModuleName, ldk::Game* game, ldk::platform::SharedLib** sharedLib)
{
	*sharedLib = ldk::platform::loadSharedLib(gameModuleName);

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

bool reloadGameModule(ldk::Game* game, ldk::platform::SharedLib** sharedLib)
{
	char* gameModuleCopyName = "_game_copy.dll";

	int64 gameModuleTime = ldk::platform::getFileWriteTime(LDK_GAME_MODULE_NAME);

	// new dll version ?
	if (gameModuleTime != lastGameDllTime)
	{
		LogInfo("Game updated. Reloading ...");
		if (lastGameDllTime != 0)
			ldk::platform::unloadSharedLib(*sharedLib);

		lastGameDllTime = gameModuleTime;
		ldk::platform::copyFile(LDK_GAME_MODULE_NAME, gameModuleCopyName);
		return loadGameModule(gameModuleCopyName, game, sharedLib);
	}

	return false;
}

GameConfig loadGameConfig()
{
	defaultConfig.width = defaultConfig.height = 600;
	defaultConfig.aspect = 1.777;
	defaultConfig.title = LDK_DEFAULT_GAME_WINDOW_TITLE;

	ldk::VariantSectionRoot* root = ldk::config_parseFile((const char8*) LDK_DEFAULT_CONFIG_FILE);

	if (root)
	{
		ldk::VariantSection* sectionDisplay =
			ldk::config_getSection(root,"display");

		if (sectionDisplay != nullptr)
		{
			ldk::config_getBool(sectionDisplay, "fullscreen", &defaultConfig.fullscreen);
			ldk::config_getInt(sectionDisplay, "width", &defaultConfig.width);
			ldk::config_getString(sectionDisplay, "title", &defaultConfig.title);
			ldk::config_getInt(sectionDisplay, "height", &defaultConfig.height);
			ldk::config_getFloat(sectionDisplay, "aspect", &defaultConfig.aspect);
			ldk::VariantSection* sectionGame = ldk::config_getSection(root,"game");
		}

	}
	return defaultConfig;
}

uint32 ldkMain(uint32 argc, char** argv)
{
	ldk::Game game = {};
	ldk::platform::SharedLib* gameSharedLib;
	GameConfig gameConfig;

	if (! ldk::platform::initialize())
	{
		LogError("Error initializing platform layer");
		return LDK_EXIT_FAIL;
	}

	gameConfig = loadGameConfig();

	uint32 windowHints[] = { 
		(uint32)ldk::platform::WindowHint::WIDTH,  gameConfig.width,
		(uint32)ldk::platform::WindowHint::HEIGHT, gameConfig.height,
		0};

	ldk::platform::LDKWindow* window =
		ldk::platform::createWindow(windowHints, gameConfig.title, nullptr);

	if (!window)
	{
		LogError("Error creating main window");
		return LDK_EXIT_FAIL;
	}

	if (!reloadGameModule(&game, &gameSharedLib))
	{
		LogError("Error loading game module");
		return LDK_EXIT_FAIL;
	}

	ldk::platform::setWindowCloseCallback(window, windowCloseCallback);
	ldk::platform::setWindowResizeCallback(window, windowResizeCallback);
	ldk::platform::toggleFullScreen(window, gameConfig.fullscreen);

	//ldk::render::setViewportAspectRatio(gameConfig.width, gameConfig.height, gameConfig.width, gameConfig.height);


	// preallocate memory for game state
	size_t gameStateMemorySize = game.init();
	void* gameStateMemory = nullptr;
  gameStateMemory = malloc(gameStateMemorySize);
  ldk::ldk_memory_set(gameStateMemory, 0, (size_t)gameStateMemorySize);

	game.start(gameStateMemory);
	float deltaTime;
	int64 startTime = 0;
	int64 endTime = 0;

	float gameReloadCheckTimeout = 0;
	while (!ldk::platform::windowShouldClose(window))
	{
		deltaTime = ldk::platform::getTimeBetweenTicks(startTime, endTime);

		startTime = ldk::platform::getTicks();
		ldk::platform::pollEvents();
		ldk::input::keyboardUpdate();
		ldk::input::mouseUpdate();
		ldk::input::joystickUpdate();

		ldkHandleKeyboardInput(window);

		//ldk::render::updateRenderer(deltaTime);
		game.update(deltaTime);
		ldk::platform::swapWindowBuffer(window);
		endTime = ldk::platform::getTicks();

		// should we reload the game dll ?
		gameReloadCheckTimeout += deltaTime;
		if (gameReloadCheckTimeout >= 5.0f)
		{
			gameReloadCheckTimeout = 0;
			if (reloadGameModule(&game, &gameSharedLib))
					game.start(gameStateMemory);
		}
	}

	game.stop();
	// release game state memory
	free(gameStateMemory);

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
