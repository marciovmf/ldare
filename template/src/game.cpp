#include <ldare/ldare_game.h>
using namespace ldare;

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define GAME_RESOLUTION_WIDTH 1024
#define GAME_RESOLUTION_HEIGHT 576
#define FULLSCREEN 0

static ldare::GameContext gameContext;
struct GameData
{
	// Game state data goes here
	Vec3 position;
} *gameMemory = nullptr;

//---------------------------------------------------------------------------
// Game Engine Initialization
//---------------------------------------------------------------------------
ldare::GameContext gameInit()
{
	gameContext.windowWidth = SCREEN_WIDTH; 								// game window width
	gameContext.windowHeight = SCREEN_HEIGHT; 							// game window height
	gameContext.Resolution.width = GAME_RESOLUTION_WIDTH;		// set game resolution width
	gameContext.Resolution.height = GAME_RESOLUTION_HEIGHT; // set game resolution height
	gameContext.gameMemorySize = sizeof(GameData);					// requested game memory size
	gameContext.fullScreen = FULLSCREEN;
	return gameContext;
}

//---------------------------------------------------------------------------
// Game start callback
//---------------------------------------------------------------------------
void gameStart(void* mem, GameApi& gameApi)
{
}

//---------------------------------------------------------------------------
// Game update
//---------------------------------------------------------------------------
void gameUpdate(const float deltaTime, const Input& input, ldare::GameApi& gameApi)
{
}

//---------------------------------------------------------------------------
// Game finalization
//---------------------------------------------------------------------------
void gameStop()
{
}

