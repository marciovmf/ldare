
#include <ldk/ldk.h>
#include "ldk_platform.h"
#include "ldk_memory.h"
#include "ldk_renderer.h"
#include "ldk_renderer_buffer.h"
// implementations
#include "ldk_memory.cpp"
#include "ldk_keyboard.cpp"
#include "ldk_gamepad.cpp"
#include "ldk_ini.cpp"
#include "ldk_asset.cpp"
// Opengl renderer dependencies
#include "ldk_renderer_gl.cpp"
#include "ldk_renderer_buffer_gl.cpp"


//TODO: use a higher level renderer interface here
//#include "ldk_renderer_gl.cpp"
#define LDK_DEFAULT_GAME_WINDOW_TITLE "LDK Window"
#define LDK_DEFAULT_CONFIG_FILE "ldk.cfg"

struct GameConfig
{
	int32 width;
	int32 height;
	bool fullscreen;
	float aspect;
	char* title;
};

void windowCloseCallback(ldk::platform::LDKWindow* window)
{
	ldk::platform::destroyWindow(window);
}

void windowResizeCallback(ldk::platform::LDKWindow* window, int32 width, int32 height)
{
	// Recalculate projection matrix here.
	return;
}

static void ldkHandleKeyboardInput(ldk::platform::LDKWindow* window, const ldk::KeyboardApi& keyboard)
{
	if (keyboard.getKeyDown(LDK_KEY_ESCAPE))
	{
		ldk::platform::setWindowCloseFlag(window, true);
	}
	
	if (keyboard.getKeyDown(LDK_KEY_F12))
	{
		ldk::platform::toggleFullScreen(window, !ldk::platform::isFullScreen(window));
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

GameConfig loadGameConfig()
{
	GameConfig defaultConfig = {};
	defaultConfig.width = defaultConfig.height = 600;
	defaultConfig.aspect = 1.777;
	defaultConfig.title = LDK_DEFAULT_GAME_WINDOW_TITLE;
	
	ldk::VariantSectionRoot* root = ldk::ldk_config_parseFile((const char8*) LDK_DEFAULT_CONFIG_FILE);

	if (root)
	{
		ldk::VariantSection* sectionDisplay =
			ldk::ldk_config_getSection(root,"display");

		if (sectionDisplay != nullptr)
		{
			ldk::ldk_config_getBool(sectionDisplay, "fullscreen", &defaultConfig.fullscreen);
			ldk::ldk_config_getInt(sectionDisplay, "width", &defaultConfig.width);
			ldk::ldk_config_getString(sectionDisplay, "title", &defaultConfig.title);
			ldk::ldk_config_getInt(sectionDisplay, "height", &defaultConfig.height);
			ldk::ldk_config_getFloat(sectionDisplay, "aspect", &defaultConfig.aspect);
		}

	}
	return defaultConfig;
}

uint32 ldkMain(uint32 argc, char** argv)
{
	ldk::Core core = {};
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

	if (!loadGameModule(&game, &gameSharedLib))
	{
		LogError("Error loading game module");
		return LDK_EXIT_FAIL;
	}

	ldk::platform::setWindowCloseCallback(window, windowCloseCallback);
	ldk::platform::setWindowResizeCallback(window, windowResizeCallback);
	ldk::platform::toggleFullScreen(window, gameConfig.fullscreen);
	ldk::ldk_keyboard_initApi(&core.keyboard);
	ldk::ldk_gamepad_initApi(&core.gamepad);

	game.init(&core);

	game.start();
	while (!ldk::platform::windowShouldClose(window))
	{
		ldk::platform::pollEvents();
		ldk::ldk_keyboard_update();
		ldk::ldk_gamepad_update();

		ldkHandleKeyboardInput(window, core.keyboard);

		game.update(0);
		ldk::render::updateRenderer(0);

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

