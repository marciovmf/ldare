#include <ldare/game.h>
#include <stdio.h>

using namespace ldare;
using namespace ldare::game;
using namespace ldare::render;

static ldare::game::GameContext gameContext;

// here goes all persistent stuff the game needs along its execution
// the goal here is to request enough memory to do whatever the game needs
// this is what most game engines does behind the scenes to make it 'friendly'
// to programmers but charges 
struct GameData
{
	Sprite sprite1;
	Sprite sprite2;
	Sprite sprite3;
	Shader shader;
	float x=0;
	float y=0;
	float step;
} *gameMemory;


//---------------------------------------------------------------------------
// Game Engine Initialization
//---------------------------------------------------------------------------
ldare::game::GameContext gameInit()
{
	gameContext.windowWidth = 600; 						// game window width
	gameContext.windowHeight = 600; 					// game window height
	gameContext.gameMemorySize = MEGABYTE(10);// requested game memory size
	gameContext.clearColor[0] = 0;
	gameContext.clearColor[1] = 1.0f;
	gameContext.clearColor[2] = 1.0f;
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

	gameMemory = (GameData*) mem;
	gameMemory->shader = gameApi.spriteBatch.loadShader(nullptr, nullptr);
	gameMemory->step = 0.008f;

	Sprite sprite;
	sprite.color = Vec3{0.0, 0.0, 1.0};
	sprite.width = sprite.height = 0.2f;
	sprite.position = Vec3{-.1, -.1, 0};
	gameMemory->sprite1 = sprite;
	
	sprite.color = Vec3{0.0, 1.0, 0.0};
	sprite.width = 0.2f;
		sprite.height = 0.5f;
	sprite.position = Vec3{0.65, 0.1, 0};
	gameMemory->sprite2 = sprite;
	
	sprite.color = Vec3{1.0, 0.0, 0.0};
	sprite.width = 0.2f;
	sprite.height = 0.2f;
	sprite.position = Vec3{-0.7, -0.2, 0};
	gameMemory->sprite3 = sprite;
}

//---------------------------------------------------------------------------
// Game update
//---------------------------------------------------------------------------

void gameUpdate(const Input& input, ldare::GameApi& gameApi)
{
	float& x = gameMemory->x;
	float& y = gameMemory->y;

	ldare::game::KeyState keyValue = input.keyboard[KBD_W];

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
	if (y < -1) y = 1;
	gameMemory->sprite1.position.x = x;
	gameMemory->sprite1.position.y = y;

	gameApi.spriteBatch.begin();
		gameApi.spriteBatch.submit(gameMemory->shader, gameMemory->sprite1);
		gameApi.spriteBatch.submit(gameMemory->shader, gameMemory->sprite2);
		gameApi.spriteBatch.submit(gameMemory->shader, gameMemory->sprite3);
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

