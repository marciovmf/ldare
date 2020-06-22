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

static bool dummy_handleEvent(const ldk::Event* event)
{
  return false;
}

bool windowEventCallback(ldk::platform::LDKWindow* window, const ldk::Event* event)
{
  // editor have priority over ESCAPE key
  if (event->type == ldk::EventType::KEYBOARD_EVENT)
  {
    if(event->keyboardEvent.key == ldk::input::LDK_KEY_ESCAPE)
    {
      ldk::platform::setWindowCloseFlag(window, true);
      return true;
    }
    if(event->keyboardEvent.key == ldk::input::LDK_KEY_F12
        && event->keyboardEvent.type == ldk::KeyboardEvent::KEYBOARD_KEY_DOWN)
    {
      bool isFullScreen = ldk::platform::isFullScreen(window);
      ldk::platform::toggleFullScreen(window, !isFullScreen);
      return true;
    }
  }

  // if we didn't handle the event, forward it to the game
  bool handledByGame = _game.onEvent(event);

  // did the game prevented us from closing ?
  if(event->type == ldk::EventType::QUIT_EVENT)
  {
    if (handledByGame) 
      ldk::platform::setWindowCloseFlag(window, false);
    else
      ldk::platform::destroyWindow(window);
    return true;
  }

  return false;
}

bool loadGameModule(char* gameModuleName, ldk::Game* game, ldk::platform::SharedLib** sharedLib)
{
  *sharedLib = ldk::platform::loadSharedLib(gameModuleName);

  if (!*sharedLib)
    return false;

  game->onSetup = (ldk::LDK_PFN_GAME_SETUP)
    ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_SETUP);
  game->onStart = (ldk::LDK_PFN_GAME_START)
    ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_START);
  game->onUpdate = (ldk::LDK_PFN_GAME_UPDATE)
    ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_UPDATE);
  game->onStop = (ldk::LDK_PFN_GAME_STOP)
    ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_STOP);
  game->onEvent = (ldk::LDK_PFN_GAME_HANDLE_EVENT)
    ldk::platform::getFunctionFromSharedLib(*sharedLib, LDK_GAME_FUNCTION_EVENT);

  // OnEvent is not mandatory, give it a dummy implementation if it is not found
  // on the game dll
  if (game->onEvent == nullptr) game->onEvent = dummy_handleEvent;

  // Log error if no mandatory functions are not found on the game dll
  if (!game->onSetup) LogError("Game dll has no required '%s' function", LDK_GAME_FUNCTION_SETUP);
  if (!game->onStart) LogError("Game dll has no required '%s' function", LDK_GAME_FUNCTION_START);
  if (!game->onUpdate) LogError("Game dll has no required '%s' function", LDK_GAME_FUNCTION_UPDATE);
  if (!game->onStop) LogError("Game dll has no required '%s' function", LDK_GAME_FUNCTION_STOP);

  return game->onSetup && game->onStart && game->onUpdate && game->onStop;
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
  LDKGameSettings gameSettings = _game.onSetup();

	uint32 windowHints[] = { 
		(uint32)ldk::platform::WindowHint::WIDTH,
    (uint32)gameSettings.displayWidth,
		(uint32)ldk::platform::WindowHint::HEIGHT,
    (uint32)gameSettings.displayHeight,
		(uint32)0};

	ldk::platform::LDKWindow* window =
		ldk::platform::createWindow(windowHints, gameSettings.name, nullptr);

  ldk::platform::showCursor(window, gameSettings.showCursor);

	if (!window)
	{
		LogError("Error creating main window");
		return LDK_EXIT_FAIL;
	}

	ldk::platform::setEventCallback(window, windowEventCallback);
	ldk::platform::toggleFullScreen(window, gameSettings.fullScreen);

	void* gameStateMemory = nullptr;
  size_t gameMemorySize = gameSettings.preallocMemorySize;
  gameStateMemory = ldk::platform::memoryAlloc(gameMemorySize);
  memset(gameStateMemory, 0, (size_t)gameMemorySize);

	_game.onStart(gameStateMemory);

	float deltaTime;
	int64 startTime = 0;
	int64 endTime = 0;

  FrameTime frameTime = {};
  ldk::Mat4 projMatrix;
  projMatrix.orthographic(0, 800, 600, 0, -10, 10);
  ldk::Vec4 textColor = {1.0f, 1.0f, 1.0f, 1.0f};
  ldk::Vec4 bgColor = {.1f, .1f, .1f, 1.0f};
  ldk::Rect viewPort = {0.0f, 0.0f, 800, 600};
  ldk::Vec3 textPos = ldk::Vec3{10, 350, 0.0f};
  ldk::HMaterial fontMaterial = ldk::loadMaterial("./assets/standard/Inconsolata_18.mat"); 
  ldk::HFont font = ldk::asset_loadFont("./assets/standard/Inconsolata_18.font"); 

	float gameReloadCheckTimeout = 0;
	while (!ldk::platform::windowShouldClose(window))
	{
		deltaTime = ldk::platform::getTimeBetweenTicks(startTime, endTime);

		startTime = ldk::platform::getTicks();
		ldk::platform::pollEvents();
		ldk::input::keyboardUpdate();
		ldk::input::mouseUpdate();
		ldk::input::joystickUpdate();

		//ldk::render::updateRenderer(deltaTime);

    frameTime.time += deltaTime;
    if (++frameTime.frameCount == 60)
    {

      float avgTime = (frameTime.time / 60) * 1000;
      if(frameTime.maxAvgTime < avgTime) frameTime.maxAvgTime = avgTime;

      //LogInfo("Max Avg = %fms, Avg %fms", frameTime.maxAvgTime, avgTime);
      
      frameTime.time = frameTime.frameCount = 0;
    }



//----------------------
    ldk::renderer::setViewPort(viewPort);
    ldk::renderer::clearBuffers(ldk::renderer::COLOR_BUFFER | ldk::renderer::DEPTH_BUFFER);
    ldk::renderer::beginFrame(projMatrix);
    ldk::renderer::spriteBatch_begin();
    ldk::renderer::spriteBatch_drawText(fontMaterial, font, textPos,
        "EDITOR TEXT!", 1.0f, textColor);
    ldk::renderer::spriteBatch_end();
    _game.onUpdate(deltaTime);
    ldk::renderer::endFrame();
//----------------------

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
  LogInfo(ldkEngine::memory_getReport());
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
