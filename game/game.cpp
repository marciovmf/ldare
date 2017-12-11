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
static Rectangle srcGroundL = { 1 * spriteSize, 0 * spriteSize, spriteSize, spriteSize};
static Rectangle srcGroundTL = {2 * spriteSize, 0, spriteSize, spriteSize};
static Rectangle srcGroundTop = {3 * spriteSize, 0, spriteSize, spriteSize};
static Rectangle srcGroundTR = { 5 * spriteSize ,  0 * spriteSize, spriteSize, spriteSize};
static Rectangle srcGroundR = { 4 * spriteSize, 0 * spriteSize, spriteSize, spriteSize};
static Rectangle srcGroundCenter = { 6 * spriteSize, 0 * spriteSize, spriteSize, spriteSize};
static Rectangle srcGroundB = { 5 * spriteSize, 1 * spriteSize, spriteSize, spriteSize};
static Rectangle srcGroundBR = { 6 * spriteSize, 1 * spriteSize, spriteSize, spriteSize};
static Rectangle srcTree = { 0 * spriteSize, 2 * spriteSize, spriteSize, spriteSize};
static Rectangle srcHero = { 3 * spriteSize, 2 * spriteSize, spriteSize, spriteSize};

struct GameData
{
	Character hero;
	Audio soundChoppTree;
	Material material;
	Material fontMaterial;
	FontAsset* fontAsset;
	Vec2 resolution;
	Vec2 scroll;
	Vec2 heroTileCoord;
	uint32 heroTileIndex;
	float zoom;
	float step;
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
				(const char*)"./assets/Caviar Dreams.bmp");
		
		gameApi.asset.loadFont(
				(const char*)"./assets/Caviar Dreams.font", &gameMemory->fontAsset );

	
	 gameApi.asset.loadAudio("./assets/chop_tree.wav", &gameMemory->soundChoppTree);

	float zoom = 1.0;
	gameMemory->zoom = zoom;

	// ground settings
	_mapSettings.width = MAP_WIDTH;
	_mapSettings.height = MAP_HEIGHT;
	_mapSettings.deathLimit = 3;
	_mapSettings.birthLimit = 4;
	_mapSettings.cellInitialAliveChance = 40; // percentage chance of starting alive
	map_initialize(_mapSettings, map);
	map_simulate(_mapSettings, map, MAP_ITERATIONS);

	// Trees settings
	_mapSettings.deathLimit = 3;
	_mapSettings.birthLimit = 5;
	_mapSettings.cellInitialAliveChance = 40;
	map_addLayer(_mapSettings, map, TREE);
	map_simulate(_mapSettings, map, 3, TREE, GROUND);

	// set map borders
	map_setMapBorders(_mapSettings, map);

	// Setup hero 
	character_setup(gameMemory->hero,
 		SPRITE_SIZE * gameMemory->zoom, 		// width
		SPRITE_SIZE * gameMemory->zoom, 		// height
		Vec3{0,0,HERO_Z}, 									// position
		Vec4{1.0, 1.0, 1.0, 1.0}); 					// color
	
	gameMemory->step =  GAME_RESOLUTION_WIDTH/_mapSettings.width;
}

//---------------------------------------------------------------------------
// draw level
//---------------------------------------------------------------------------
void drawMap(GameApi& gameApi, MapSettings& settings, MapNode* map, Vec2& scroll, 
		float zoom, uint32 tileType, float minZ, Rectangle& srcSprite, Vec2& scale = Vec2{1,1})
{
	float zoomedSize = spriteSize * zoom;
	float stepX = zoomedSize;
	float stepY = zoomedSize;
	
	Sprite sprite;
	sprite.width = stepX * scale.x;
	sprite.height = stepY * scale.y;
	sprite.srcRect = srcSprite;
	sprite.position.z = minZ;
	sprite.color = Vec4{1.0, 1.0, 1.0, 1.0};

		for(uint32 x=0; x < settings.width; x++)
		{
			for(uint32 y=0; y < settings.height; y++)
			{
				uint32 pos = x + settings.height * y;
				MapNode cellValue = map[pos];

				if (!(cellValue.type & tileType))
					continue;

				float halfWidth = GAME_RESOLUTION_WIDTH/2;
				float halfHeight = GAME_RESOLUTION_HEIGHT/2;

					//HACK: remove this. This is for testing map borders
				if (tileType & GROUND == GROUND)
				{
					if (cellValue.border == BORDER_TOP)
						sprite.srcRect = srcGroundTop;
					else if (cellValue.border == BORDER_BOTTOM)
						sprite.srcRect = srcGroundB;
					else if (cellValue.border == BORDER_LEFT)
						sprite.srcRect = srcGroundL;
					else if (cellValue.border == BORDER_RIGHT)
						sprite.srcRect = srcGroundR;
					else if (cellValue.border == (uint16)(BORDER_TOP | BORDER_LEFT))
						sprite.srcRect = srcGroundTL;
					else if (cellValue.border == (uint16)(BORDER_TOP | BORDER_RIGHT))
						sprite.srcRect = srcGroundTR;
					else if (cellValue.border == (uint16)(BORDER_BOTTOM | BORDER_LEFT))
						sprite.srcRect = srcGroundBL;
					else if (cellValue.border == (uint16)(BORDER_BOTTOM | BORDER_RIGHT))
						sprite.srcRect = srcGroundBR;
					else
						sprite.srcRect = srcSprite;
				}


				sprite.position = { stepX * x, (stepY * y), sprite.position.z };// + 0.0001 * y};

				// center the map around screen origin and scroll
				sprite.position.x += scroll.x; 
				sprite.position.y += scroll.y;
				
				// submit the ground sprite
				gameApi.spriteBatch.submit(sprite);
			}
		}
}

static uint32 getTileFromPixel(Vec3& position, Vec2 scroll, uint32 spriteSize)
{
	const float halfSprite = spriteSize/2;

		int32 tileX = (position.x + halfSprite - scroll.x) / spriteSize;
		int32 tileY = (position.y + halfSprite - scroll.y) / spriteSize;
		return (uint32) (tileX + _mapSettings.width * tileY);
}

//---------------------------------------------------------------------------
// Game update
//---------------------------------------------------------------------------
void gameUpdate(const float deltaTime, const Input& input, ldare::GameApi& gameApi)
{
	gameApi.spriteBatch.flush();
	gameApi.text.flush();
	Vec2& scroll = gameMemory->scroll;
	bool update = false;

	if (input.getKeyDown(KBD_I))
	{
		gameMemory->zoom = MIN(abs(gameMemory->zoom + 1), MAX_ZOOM);
	}
	else if (input.getKeyDown(KBD_O))
	{
		gameMemory->zoom = MAX( gameMemory->zoom - 1, 1.0f);
	}

	character_update(gameMemory->hero, input, gameApi, deltaTime);
	
	if (gameMemory->hero.isWalking)
	{
		float speed = SPRITE_SIZE * 3 * deltaTime;

		Vec3& heroPosition = gameMemory->hero.sprite.position;
		Vec3& heroDirection = gameMemory->hero.direction;

		const float halfSprite = SPRITE_SIZE/2;
		float hMovementLimitMin = GAME_RESOLUTION_WIDTH /4 - halfSprite;
		float hMovementLimitMax = (GAME_RESOLUTION_WIDTH /4 - halfSprite) * 4;

		float vMovementLimitMin = GAME_RESOLUTION_HEIGHT /4  - halfSprite;
		float vMovementLimitMax = (GAME_RESOLUTION_HEIGHT /4  - halfSprite) * 4;

		// Find the tile the character is on
		gameMemory->heroTileIndex = getTileFromPixel(heroPosition, gameMemory->scroll, SPRITE_SIZE);
		if (map[gameMemory->heroTileIndex].type != GROUND)
		{
			speed = speed /2;
		}

		// move the character OR the map
		// HORIZONTAL
		if ( heroPosition.x < hMovementLimitMin)
		{
			heroPosition.x = hMovementLimitMin;
			scroll.x -= (speed * gameMemory->hero.direction.x);
		}
		else if ( heroPosition.x > hMovementLimitMax)
		{
			heroPosition.x = hMovementLimitMax;
			scroll.x -= (speed * gameMemory->hero.direction.x);
		}
		else
		{
			heroPosition.x += speed * heroDirection.x;
		}
		// VERTICAL
		if ( heroPosition.y < vMovementLimitMin)
		{
			heroPosition.y = vMovementLimitMin;
			scroll.y += (speed * -gameMemory->hero.direction.y);
		}
		else if ( heroPosition.y > vMovementLimitMax)
		{
			heroPosition.y = vMovementLimitMax;
			scroll.y += (speed * -gameMemory->hero.direction.y);
		}
		else
		{
			heroPosition.y += speed * heroDirection.y;
		}

		}

	// Opaque objects first
	gameApi.spriteBatch.begin(gameMemory->material);
		drawMap(gameApi, _mapSettings, map, gameMemory->scroll, gameMemory->zoom, GROUND | TREE, GROUND_Z, srcGroundCenter);
		gameApi.spriteBatch.submit(gameMemory->hero.sprite);
		drawMap(gameApi, _mapSettings, map, gameMemory->scroll, gameMemory->zoom, TREE, TREE_Z, srcTree);
	gameApi.spriteBatch.end();

	gameApi.text.begin(*gameMemory->fontAsset, gameMemory->fontMaterial);
		gameApi.text.drawText(Vec3{100,100,1}, 1.0f, Vec4{0.0f, 0.0f, 0.0f, 1.0f}, "Hello World!");
	gameApi.text.end();
}

//---------------------------------------------------------------------------
// Game finalization
//---------------------------------------------------------------------------
void gameStop()
{
	LogInfo("Game stopped");
}

