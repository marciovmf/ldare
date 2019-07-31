#include <ldk/ldk.h>
#include <ldkengine/ldk_platform.h>
#include  <ldkengine/ldk_memory.h>

#include <string.h> // for memset()

struct FrameTime
{
  uint32 frameCount;
  float maxAvgTime;
  float time;
};

static int64 lastGameDllTime = 0;
static ldk::Game _game = {};

void windowCloseCallback(ldk::platform::LDKWindow* window)
{
	ldk::platform::destroyWindow(window);
}

void windowResizeCallback(ldk::platform::LDKWindow* window, int32 width, int32 height)
{
  if (_game.onViewResized != nullptr)
  {
    _game.onViewResized(width, height);
  }
	return;
}

static void ldkHandleKeyboardInput(ldk::platform::LDKWindow* window)
{
  if (ldk::input::isKeyDown(ldk::input::LDK_KEY_ESCAPE))
  {
    ldk::platform::setWindowCloseFlag(window, true);
  }

  if (ldk::input::isKeyDown(ldk::input::LDK_KEY_F12))
  {
    ldk::platform::toggleFullScreen(window, !ldk::platform::isFullScreen(window));
  }

#ifdef _LDK_DEBUG_
  if (ldk::input::isKeyDown(ldk::input::LDK_KEY_F2))
  {
    ldkEngine::memory_printReport();
  }
#endif
}

bool loadGameModule(char* gameModuleName, ldk::Game* game, ldk::platform::SharedLib** sharedLib)
{
  *sharedLib = ldk::platform::loadSharedLib(gameModuleName);

  if (!*sharedLib)
    return false;

  game->onInit = (ldk::LDK_PFN_GAME_INIT)
    ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_INIT);
  game->onStart = (ldk::LDK_PFN_GAME_START)
    ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_START);
  game->onUpdate = (ldk::LDK_PFN_GAME_UPDATE)
    ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_UPDATE);
  game->onStop = (ldk::LDK_PFN_GAME_STOP)
    ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_STOP);
  game->onViewResized = (ldk::LDK_PFN_GAME_VIEW_RESIZED)
    ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_VIEW_RESIZED);

  return game->onInit && game->onStart && game->onUpdate && game->onStop;
}

bool reloadGameModule(ldk::Game* game, ldk::platform::SharedLib** sharedLib)
{
	char* gameModuleCopyName = "_game_copy.dll";

	int64 gameModuleTime = ldk::platform::getFileWriteTime(LDK_GAME_MODULE_NAME);

	// new dll version ?
	if (gameModuleTime != lastGameDllTime)
	{
    if (lastGameDllTime == 0)
      LogInfo("Loading game module.");
    else
      LogInfo("Game module recompiled. Reloading ...");

		if (lastGameDllTime != 0)
			ldk::platform::unloadSharedLib(*sharedLib);

		lastGameDllTime = gameModuleTime;
		ldk::platform::copyFile(LDK_GAME_MODULE_NAME, gameModuleCopyName);
		return loadGameModule(gameModuleCopyName, &_game, sharedLib);
	}

	return false;
}

uint32 ldkMain(uint32 argc, char** argv)
{
	ldk::platform::SharedLib* gameSharedLib;

  if (! ldk::platform::initialize())
	{
		LogError("Error initializing platform layer");
		return LDK_EXIT_FAIL;
	}

	if (!reloadGameModule(&_game, &gameSharedLib))
	{
		LogError("Error loading game module");
		return LDK_EXIT_FAIL;
	}

	// preallocate memory for game state
  LDKGameSettings gameSettings = _game.onInit();

	uint32 windowHints[] = { 
		(uint32)ldk::platform::WindowHint::WIDTH,
    (uint32)gameSettings.displayWidth,
		(uint32)ldk::platform::WindowHint::HEIGHT,
    (uint32)gameSettings.displayHeight,
		(uint32)0};

	ldk::platform::LDKWindow* window =
		ldk::platform::createWindow(windowHints, gameSettings.name, nullptr);

	if (!window)
	{
		LogError("Error creating main window");
		return LDK_EXIT_FAIL;
	}

	ldk::platform::setWindowCloseCallback(window, windowCloseCallback);
	ldk::platform::setWindowResizeCallback(window, windowResizeCallback);
	ldk::platform::toggleFullScreen(window, gameSettings.fullScreen);

	//ldk::render::setViewportAspectRatio(gameSetting.width, gameSetting.height, gameSetting.width, gameSetting.height);

	void* gameStateMemory = nullptr;
  size_t gameMemorySize = gameSettings.preallocMemorySize;
  gameStateMemory = ldk::platform::memoryAlloc(gameMemorySize);
  memset(gameStateMemory, 0, (size_t)gameMemorySize);

	_game.onStart(gameStateMemory);
	float deltaTime;
	int64 startTime = 0;
	int64 endTime = 0;

  FrameTime frameTime = {};

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

    frameTime.time += deltaTime;
    if (++frameTime.frameCount == 60)
    {

      float avgTime = ( frameTime.time / 60 ) * 1000;
      if(frameTime.maxAvgTime < avgTime) frameTime.maxAvgTime = avgTime;

      //LogInfo("Max Avg = %fms, Avg %fms", frameTime.maxAvgTime, avgTime);
      
      frameTime.time = frameTime.frameCount = 0;
    }

		_game.onUpdate(deltaTime);
		ldk::platform::swapWindowBuffer(window);
		endTime = ldk::platform::getTicks();

		// should we reload the game dll ?
		gameReloadCheckTimeout += deltaTime;
		if (gameReloadCheckTimeout >= 5.0f)
		{
			gameReloadCheckTimeout = 0;
			if (reloadGameModule(&_game, &gameSharedLib))
					_game.onStart(gameStateMemory);
		}
	}

	_game.onStop();

	// release game state memory
  ldk::platform::memoryFree(gameStateMemory);

#ifdef _LDK_DEBUG_
  LogInfo("Game stopped.");
  ldkEngine::memory_printReport();
#endif

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
