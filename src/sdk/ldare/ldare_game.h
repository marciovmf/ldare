/**
 * game.h
 * Standard header for ldare based games
 */
#ifndef __LDARE_GAME__
#define __LDARE_GAME__
#include <ldare/ldare.h>

namespace ldare 
{
	//---------------------------------------------------------------------------
	// Runtime settings the game can return to the engine 
	// during initialization
	//---------------------------------------------------------------------------
	struct GameContext
	{
		int32 windowWidth;
		int32 windowHeight;
		size_t gameMemorySize; 				// Amount of memory allocated to the game
		struct 
		{
			uint32 width;
			uint32 height;
		} Resolution;
		float clearColor[3];
		bool fullScreen;
	};
} // ldare namespace

// API exposed to the game
namespace ldare
{
	struct GameApi
	{
		ldare::SpriteBatchApi spriteBatch;
		ldare::AssetApi asset;
		ldare::AudioApi audio;
		ldare::TextBatchApi text;
	};
}

//---------------------------------------------------------------------------
// Game Interface
// This interface is used by the engine to communicate with the game
//---------------------------------------------------------------------------

#define GAME_INIT_FUNC(name) ldare::GameContext (name)()
typedef GAME_INIT_FUNC(gameInitFunc);

#define GAME_START_FUNC(name) void (name)(void* gameMemory, ldare::GameApi& gameApi)
typedef GAME_START_FUNC(gameStartFunc);

#define GAME_UPDATE_FUNC(name) void (name)(const float deltaTime, const ldare::Input& input, ldare::GameApi& gameApi)
typedef GAME_UPDATE_FUNC(gameUpdateFunc);

#define GAME_STOP_FUNC(name) void (name)();
typedef GAME_STOP_FUNC(gameStopFunc)

#endif // __LDARE_GAME__
