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
	Sprite sprite;
	Shader shader;
} *gameMemory;

//---------------------------------------------------------------------------
// Game Engine Initialization
//---------------------------------------------------------------------------
ldare::game::GameContext gameInit()
{
	gameContext.windowWidth = 800; 						// game window width
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
void gameStart(void* mem)
{
	LogInfo("Game started");
	if(!mem)
	{
		LogError("initial memory allocation failed");
	}
	gameMemory = (GameData*) mem;

	Sprite sprite;
	sprite.width = sprite.height = 0.3f;
	sprite.position = Vec3{0.2, 0.2, 0};
	
	gameMemory->sprite = sprite;
	gameMemory->shader = render::loadShader(nullptr, nullptr);
}

//---------------------------------------------------------------------------
// Game update
//---------------------------------------------------------------------------
void gameUpdate(const Input& input)
{
	ldare::game::KeyState keyValue = input.keyboard[KBD_W];

	if(keyValue.state)
	{
		if (keyValue.thisFrame)
		{
			LogInfo("DOWN");
		}
		else
		{
			LogInfo("HOLDING");
		}
	}
	else 	
	{
		if(keyValue.thisFrame)
			LogInfo("UP");
	}
	render::begin();
	render::submit(gameMemory->shader, gameMemory->sprite);
	render::end();
	render::flush();
}

//---------------------------------------------------------------------------
// Game finalization
//---------------------------------------------------------------------------
void gameStop()
{
	LogInfo("Game stopped");
}

