#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define GAME_RESOLUTION_WIDTH 1024
#define GAME_RESOLUTION_HEIGHT 576
#define FULLSCREEN 0
#define SPRITE_SIZE 128
#define MAP_WIDTH 30
#define MAP_HEIGHT 30
#define MAP_ITERATIONS 3
#define MAX_ZOOM 3

#define GROUND_Z 0.1
#define HERO_Z 0.2
#define TREE_Z 0.3

#define WATER 0x00
#define GROUND 0x1
#define TREE 0x10

#include <ldare/ldare_game.h>
#include <wchar.h>
#include <stdio.h>

using namespace ldare;
#include "procedural_map.cpp"
#include "animation.cpp"

#include "character.cpp"

// Globals
MapSettings _mapSettings;

static ldare::GameContext gameContext;
// sprite rects
static float spriteSize = SPRITE_SIZE;
static Rectangle srcGroundBL = { 0 * spriteSize, 0 * spriteSize, spriteSize, spriteSize};
struct GameData
{
	Audio soundFx;
	Material material;
	Material fontMaterial;
	FontAsset* fontAsset;
	Vec2 resolution;
} *gameMemory = nullptr;

// reserve 3 more lines for simulation routine
MapNode map[(MAP_WIDTH * MAP_HEIGHT)] = {};

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

	gameMemory->fontMaterial = gameApi.asset.loadMaterial(
			(const char*)"./assets/font.vert", 
			(const char*) "./assets/font.frag", 
			(const char*)"./assets/Liberation Mono.bmp");

	gameApi.asset.loadFont(
			(const char*)"./assets/Liberation Mono.font", &gameMemory->fontAsset);

	gameApi.asset.loadAudio("./assets/bumpSound.wav", &gameMemory->soundFx);
}

//---------------------------------------------------------------------------
// Game update
//---------------------------------------------------------------------------
Vec3 textPosition = {1, 1, 1};
Vec3 direction = {1, 1, 1};
float deltaSum =0;

void gameUpdate(const float deltaTime, const Input& input, ldare::GameApi& gameApi)
{
	gameApi.text.flush();
	uchar8 playSound=0;

	// Calculate colors for text
	float r =  textPosition.x / GAME_RESOLUTION_WIDTH;
	float g =  textPosition.y / GAME_RESOLUTION_HEIGHT;
	float b = 1 - deltaSum;
	
	deltaSum += 0.1 * deltaTime;
	if (deltaSum > 1)	deltaSum = 0;

	Vec4 color = { r, g, b ,1};

	// Render the text
	gameApi.text.begin(*gameMemory->fontAsset, gameMemory->fontMaterial);
		Vec2 textSize = gameApi.text.drawText(textPosition, 1.0f, color, "{ Hello LDARE! }");		
	gameApi.text.end();
	
	// Update text position and bounce on screen edges
	// Vertical bouncing
	if (textPosition.y <=0)
	{
		textPosition.y = 1;
		direction.y *=-1;
		playSound=1;
	}
	else if ((textPosition.y + textSize.y) >= GAME_RESOLUTION_HEIGHT)
	{
		textPosition.y = GAME_RESOLUTION_HEIGHT - textSize.y;
		direction.y *=-1;
		playSound=1;
	}
	// Horizontal bouncing
	if (textPosition.x + textSize.x >= GAME_RESOLUTION_WIDTH) 
	{
		textPosition.x = GAME_RESOLUTION_WIDTH - textSize.x - 1;
		direction.x *= -1;
		playSound=1;
	}
	else if (textPosition.x <= 0) 
	{
		textPosition.x = 1;
		direction.x *= -1;
		playSound=1;
	}

	// Make some noise when it bounces
	if (playSound)
	{
		gameApi.audio.playAudio(&gameMemory->soundFx);
	}
	
	textPosition += (direction * 100 * deltaTime);
	textPosition.z = 1;
}

//---------------------------------------------------------------------------
// Game finalization
//---------------------------------------------------------------------------
void gameStop()
{
	LogInfo("Game stopped");
}

