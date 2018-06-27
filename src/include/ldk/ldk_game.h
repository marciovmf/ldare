/**
 * game.h
 * Standard header for ldare based games
 */
#ifndef _LDK_GAME_H_
#define _LDK_GAME_H_

// API exposed to the game
namespace ldk
{

#define LDK_GAME_FUNCTION_INIT "gameInit"
#define LDK_GAME_FUNCTION_START "gameStart"
#define LDK_GAME_FUNCTION_UPDATE "gameUpdate"
#define LDK_GAME_FUNCTION_STOP "gameStop"

	typedef void (*LDK_PFN_GAME_INIT)(void* memory);
	typedef void (*LDK_PFN_GAME_START)();
	typedef void (*LDK_PFN_GAME_UPDATE)(float deltaTime);
	typedef void (*LDK_PFN_GAME_STOP)();

	struct Game
	{
		LDK_PFN_GAME_INIT init;
		LDK_PFN_GAME_START start;
		LDK_PFN_GAME_UPDATE update;
		LDK_PFN_GAME_STOP stop;
	};
}

#if defined(_LDK_WINDOWS_)
#	define LDK_GAME_MODULE_NAME "ldk_game.dll"
#else
#		error "Undefined platform"
#endif

extern "C"
{
	LDK_GAME_CALLBACK void gameInit(void* gameStateMemory);
	LDK_GAME_CALLBACK void gameStart();
	LDK_GAME_CALLBACK void gameUpdate(float deltaTime);
	LDK_GAME_CALLBACK void gameStop();
}

#endif // _LDK_GAME_H_
