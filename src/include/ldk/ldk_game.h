#ifndef _LDK_GAME_H_
#define _LDK_GAME_H_

#if defined(_LDK_WINDOWS_)
#define LDK_GAME_MODULE_NAME "ldk_game.dll"
#else
#error "Undefined platform"
#endif

struct LDKGameSettings
{
  const char* name;
  ///@brief If this value is greater than zero, the engine will allocate this much
  /// memory for the game and return a pointer to this memory when calling
  //gameStart(); You can use this memory to whatever you want. This memory
  //will remain intact when hot reloading the game after recompiling it.
  size_t preallocMemorySize;
  /// Should the game be initialized in fullscreen
  bool fullScreen;
  /// Should the system cursor be displayed
  bool showCursor;
  /// Game aspect ratio
  float aspect;
  /// Screen Resolution width
  int32 displayWidth;
  /// Screen Resolution height
  int32 displayHeight;
};

/// @defgroup Game Game
/// @details These are mandatory functions that must be implemented when creating a new game.
///A game must be a shared library named *ldk_game* that exposes all of the functions documented
///here. 
/// @{

///@brief Game settings passed to the engine during initialization.
//The engine use this struct to initialize basic settings for the game.

extern "C"
{
  ///@brief Game initialization callback. This function is called by the engine
  ///when so the game car ask the engine to initialize certain elements like
  ///screen size and reserve a block of memory.
  ///@returns The desired initiali game settings
  /// You can build the settings manually or load it from th game.cfg file. 
  ///@see loadGameSettings for loading this from a file
  LDK_GAME_CALLBACK LDKGameSettings onInit();

  ///@brief Game start callback. This finction is called by the engine right
  ///after gameInit();
  ///@param memory - The preallocated memory buffer, according to
  ///GameSettings.preallocMemorySize value returned by gameInit();
  LDK_GAME_CALLBACK void onStart(void* memory);

  ///@brief Game update callbac. Tthis function is called by the engine every
  ///frame.
  ///@param deltaTime - The time in seconds since the last frame.
  LDK_GAME_CALLBACK void onUpdate(float deltaTime);

  ///@brief Game stop callback. This function is called by the engine before
  ///terminating the game. Use this for finalziation and releasing any allocated
  ///resources. Note that you must not manually release the preallocated memory
  ///received by gameStart();
  LDK_GAME_CALLBACK void onStop();
 

  ///@brief Game event callback. This function is called by the engine 
  ///whenever an event happens. Use this for handling events.
  LDK_GAME_CALLBACK bool onEvent(const ldk::Event* event);

  ///@brief Game stop callback. This function is called by the engine when the
  //game display is resized.
  //
  //LDK_GAME_CALLBACK void gameViewResized(uint32 width, uint32 height);

  namespace ldk
  {
    ///@brief This utility function loads a the GameSettings from a file called
    /// ldk_game.cfg on the same folder as the game application.
    LDK_API LDKGameSettings loadGameSettings();
  }

}
/// @}

#define LDK_GAME_FUNCTION_INIT "onInit"
#define LDK_GAME_FUNCTION_START "onStart"
#define LDK_GAME_FUNCTION_UPDATE "onUpdate"
#define LDK_GAME_FUNCTION_STOP "onStop"
#define LDK_GAME_FUNCTION_EVENT "onEvent"

// API exposed to the game
namespace ldk
{
  typedef LDKGameSettings (*LDK_PFN_GAME_INIT)();
  typedef void (*LDK_PFN_GAME_START)(void* memory);
  typedef void (*LDK_PFN_GAME_UPDATE)(float deltaTime);
  typedef void (*LDK_PFN_GAME_STOP)();
  typedef bool (*LDK_PFN_GAME_HANDLE_EVENT)(const ldk::Event* event);

  struct Game
  {
    LDK_PFN_GAME_INIT onInit;
    LDK_PFN_GAME_START onStart;
    LDK_PFN_GAME_UPDATE onUpdate;
    LDK_PFN_GAME_STOP onStop;
    LDK_PFN_GAME_HANDLE_EVENT onEvent;
  };
}

#endif // _LDK_GAME_H_
