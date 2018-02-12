#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define GAME_RESOLUTION_WIDTH 1024
#define GAME_RESOLUTION_HEIGHT 576
#define FULLSCREEN 0
#define SPRITE_SIZE 128
#define WATER 0x00
#define GROUND 0x1
#define TREE 0x10

#include <ldare/ldare_game.h>
#include <wchar.h>
#include <stdio.h>

using namespace ldare;
#include "animation.cpp"
#include "character.cpp"


static ldare::GameContext gameContext;
// sprite rects
static float spriteSize = SPRITE_SIZE;
static Rectangle srcGroundBL = { 0 * spriteSize, 0 * spriteSize, spriteSize, spriteSize};
struct GameData
{
	Audio soundFx;
	Material material;
	FontAsset* fontAsset;
	Vec2 resolution;
	Character character;
} *gameMemory = nullptr;

//---------------------------------------------------------------------------
// Game Engine Initialization
//---------------------------------------------------------------------------
ldare::GameContext gameInit()
{
	gameContext.windowWidth = SCREEN_WIDTH; 						// game window width
	gameContext.windowHeight = SCREEN_HEIGHT; 					// game window height
	gameContext.Resolution.width = GAME_RESOLUTION_WIDTH;
	gameContext.Resolution.height = GAME_RESOLUTION_HEIGHT;
	gameContext.gameMemorySize = sizeof(GameData);// requested game memory size
	gameContext.fullScreen = FULLSCREEN;
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
	}
	// load material
	gameMemory->material = gameApi.asset.loadMaterial(
			(const char*)"./assets/sprite.vert",
			(const char*) "./assets/sprite.frag", 
			(const char*)"./assets/spritesheet.bmp");

	Vec3 pos = {GAME_RESOLUTION_WIDTH/2, GAME_RESOLUTION_HEIGHT/2, 1};
	Vec4 color = {1.0, 1.0, 1.0, 1.0};
	character_setup(gameMemory->character, 
			SPRITE_SIZE, SPRITE_SIZE, 
			pos, color);
}

//---------------------------------------------------------------------------
// Game update
//---------------------------------------------------------------------------
Vec3 textPosition = {1, 1, 1};
Vec3 direction = {1, 1, 1};
float deltaSum =0;
const float speed = 150;
void gameUpdate(const float deltaTime, const Input& input, ldare::GameApi& gameApi)
{
	gameApi.text.flush();
	Sprite& heroSprite = gameMemory->character.sprite;

	// Updata character animation
	character_update(gameMemory->character, input, gameApi, deltaTime);


	if (input.getKey(KBD_D))
	{
		heroSprite.position.x += speed * deltaTime;
	}
	else if (input.getKey(KBD_A))
	{
		heroSprite.position.x -= speed * deltaTime;
	}
	
	if (input.getKey(KBD_W))
	{
		heroSprite.position.y += speed * deltaTime;
	}
	else if (input.getKey(KBD_S))
	{
		heroSprite.position.y -= speed * deltaTime;
	}
	gameMemory->character.sprite.angle += RADIAN(180) * deltaTime;
	gameApi.spriteBatch.begin(gameMemory->material);
	gameApi.spriteBatch.submit(heroSprite);
	gameApi.spriteBatch.end();
}

//---------------------------------------------------------------------------
// Game finalization
//---------------------------------------------------------------------------
void gameStop()
{
	LogInfo("Game stopped");
}

