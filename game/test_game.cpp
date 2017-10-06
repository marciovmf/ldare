#include <ldare/ldare_game.h>
#include <stdio.h>

using namespace ldare;
static ldare::GameContext gameContext;

// here goes all persistent stuff the game needs along its execution
// the goal here is to request enough memory to do whatever the game needs
// this is what most game engines does behind the scenes to make it 'friendly'
// to programmers but charges 
struct GameData
{
	Sprite bg;
	Material material;
	float x=0;
	float y=0;
	float step;
} *gameMemory = nullptr;

//---------------------------------------------------------------------------
// Game Engine Initialization
//---------------------------------------------------------------------------
ldare::GameContext gameInit()
{
	gameContext.windowWidth = 600; 						// game window width
	gameContext.windowHeight = 600; 					// game window height
	gameContext.gameMemorySize = MEGABYTE(10);// requested game memory size
	//gameContext.fullScreen =true;
	return gameContext; 											// let the engine know what we want
}

//---------------------------------------------------------------------------
// Game start callback
//---------------------------------------------------------------------------
void gameStart(void* mem, GameApi& gameApi)
{
	LogInfo("Game started");
	if(!mem)
	{
		LogError("initial memory allocation failed");
	}

	if ( gameMemory == nullptr)
	{
		gameMemory = (GameData*) mem;
		gameMemory->step = 0.008f;
		gameMemory->x = gameMemory->y = 0;
		// load material
		gameMemory->material = gameApi.asset.loadMaterial(nullptr, nullptr, 
				(const char8*)"./assets/sprite.bmp");
	}

	Sprite sprite;
	sprite.color = Vec3{0.0f, 0.0f, 1.0f};
	sprite.width = 
		sprite.height = 0.4f;
	sprite.position = Vec3{0.0f ,0.0f ,0.0f};
	gameMemory->bg = sprite;
}

//---------------------------------------------------------------------------
// Game update
//---------------------------------------------------------------------------

void gameUpdate(const Input& input, ldare::GameApi& gameApi)
{
	float& x = gameMemory->x;
	float& y = gameMemory->y;

	if ( input.keyboard[KBD_W].state )
		y += gameMemory->step;

	if ( input.keyboard[KBD_S].state )
		y -= gameMemory->step;

	if ( input.keyboard[KBD_A].state )
		x -= gameMemory->step;

	if ( input.keyboard[KBD_D].state )
		x += gameMemory->step;

	if (x >1) x = -1;
	if (x <-1) x = 1;
	if (y >1) y = -1;
	if (y <-1) y = 1;
	gameMemory->bg.position.x = x;
	gameMemory->bg.position.y = y;

	gameApi.spriteBatch.begin();
		gameApi.spriteBatch.submit(gameMemory->material, gameMemory->bg);
	gameApi.spriteBatch.end();
	gameApi.spriteBatch.flush();

}

//---------------------------------------------------------------------------
// Game finalization
//---------------------------------------------------------------------------
void gameStop()
{
	LogInfo("Game stopped");
}

