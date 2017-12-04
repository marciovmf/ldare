#include <ldare/ldare_game.h>
using namespace ldare;
#include "procedural_map.cpp"
#include "animation.cpp"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define GAME_RESOLUTION_WIDTH 1024
#define GAME_RESOLUTION_HEIGHT 576
#define FULLSCREEN 0
#define SPRITE_SIZE 128
#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define MAP_ITERATIONS 3
#define MAX_ZOOM 3
#define LEVEL_Z 0.1
#define HERO_Z 0.2
#define TREE_Z 0.3

#define SAND 1
#define TREE 3

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
	Vec2 resolution;
	Character hero;
	Audio soundChoppTree;
	Material material;
	float x;
	float y;
	float zoom;
	float step;
	Sprite tiles[50];
} *gameMemory = nullptr;

// reserve 3 more lines for simulation routine
uint8 map[(MAP_WIDTH * MAP_HEIGHT)] = {};

//---------------------------------------------------------------------------
// Count alive neighbours around a given cell
//---------------------------------------------------------------------------
void map_simulate(MapSettings& settings, uint8* map, uint32 simulations, uint8 alive=1, uint8 dead=0)
{
	uint8* snapshot = new uint8[settings.width * settings.height];
	uint32 mapSize = settings.width * settings.height;
	for (int32 i=0; i < simulations; i++)
	{
		// take a snapshot of the original map
		memcpy(snapshot, map, mapSize);

		for (int32 x=0; x < settings.width; x++)
		{
			for (int32 y=0; y < settings.height; y++)
			{
				uint32 neighbourCount = map_countNeighbours(settings, snapshot, x, y, alive);
				uint32 cellIndex = x * settings.width + y;

				// is it alive now ?
				if (snapshot[cellIndex] == alive)
				{
					if (neighbourCount < settings.deathLimit)
						map[cellIndex] = dead;
					else
						map[cellIndex] = alive;
				}
				else if (snapshot[cellIndex] == dead)
				{
					if ( neighbourCount >= settings.birthLimit)
						map[cellIndex] = alive;
					else
						map[cellIndex] = dead;
				}
			}
		}
	}
	//HACK: for testing collision
	uint32 hackTree = 1 * settings.width + 1;
	LogInfo("hackTree at %d",hackTree);
	map[hackTree] = 3;
	delete snapshot;
}

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
		gameMemory->x = gameMemory->y = 0;
		// load material
		gameMemory->material = gameApi.asset.loadMaterial(
				(const char*)"./assets/sprite.vert", 
				(const char*) "./assets/sprite.frag", 
				(const char*)"./assets/spritesheet.bmp");
	
	 gameApi.asset.loadAudio("./assets/chop_tree.wav", &gameMemory->soundChoppTree);
	}

	float zoom = 1.0;
	gameMemory->zoom = zoom;
	gameMemory->x = gameMemory->y = 0;



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
	map_simulate(_mapSettings, map, 3, TREE, SAND);

	// Setup hero 
	character_setup(gameMemory->hero,
 		SPRITE_SIZE * gameMemory->zoom, 			// width
		SPRITE_SIZE * gameMemory->zoom, 		// height
		Vec3{
		 //(GAME_RESOLUTION_WIDTH/2 - SPRITE_SIZE/2) * zoom,
		 //(GAME_RESOLUTION_HEIGHT/2 - SPRITE_SIZE/2) * zoom, 
		 0,0,
		 HERO_Z}, 	// position
		Vec4{1.0f, 0.0f, 0.0f, 1.0f}); 																		// color

	gameMemory->step =  GAME_RESOLUTION_WIDTH/_mapSettings.width;
}

//---------------------------------------------------------------------------
// draw level
//---------------------------------------------------------------------------
void drawMap(GameApi& gameApi, MapSettings& settings, uint8* map, float zoom, float scrollX, float scrollY)
{
	float zoomedSize = spriteSize * zoom;
	float stepX = zoomedSize;
	float stepY = zoomedSize;
	Sprite sprite;
	sprite.width = stepX;
	sprite.height = stepY;

	for(uint32 x=0; x < settings.width; x++)
	{
		for(uint32 y=0; y < settings.height; y++)
		{
			uint32 pos = x * settings.width + y;
			uint8 cellValue = map[pos];

			if (cellValue == 0)
				continue;

			sprite.srcRect = srcGroundCenter;

			float halfWidth = GAME_RESOLUTION_WIDTH/2;
			float halfHeight = GAME_RESOLUTION_HEIGHT/2;

			//sprite.position = { stepX * x,  GAME_RESOLUTION_HEIGHT - stepY - (stepY * y), LEVEL_Z};
			sprite.position = { stepX * x, (stepY * y), LEVEL_Z};
	
			// center the map around screen origin and scroll
			sprite.position.x += scrollX; 
			sprite.position.y += scrollY;

			// submit the ground sprite
			gameApi.spriteBatch.submit(sprite);

			// submit the tree sprite if applicable
			if ( cellValue == TREE)
			{
				sprite.srcRect = srcTree;
				sprite.position.z = TREE_Z + 0.0001f * y;
				gameApi.spriteBatch.submit(sprite);
			}
		}
	}
}

//---------------------------------------------------------------------------
// Game update
//---------------------------------------------------------------------------
Animation* currentAnimation = nullptr;
bool isChopping;
float remainingChoppTime;
void gameUpdate(const float deltaTime, const Input& input, ldare::GameApi& gameApi)
{
	gameApi.spriteBatch.flush();
	float& scrollX = gameMemory->x;
	float& scrollY = gameMemory->y;
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

		// move the character OR the map
		// HORIZONTAL
		if ( heroPosition.x < hMovementLimitMin)
		{
			heroPosition.x = hMovementLimitMin;
			scrollX -= (speed * gameMemory->hero.direction.x);
		}
		else if ( heroPosition.x > hMovementLimitMax)
		{
			heroPosition.x = hMovementLimitMax;
			scrollX -= (speed * gameMemory->hero.direction.x);
		}
		else
		{
			heroPosition.x += speed * heroDirection.x;
		}
		// VERTICAL
		if ( heroPosition.y < vMovementLimitMin)
		{
			heroPosition.y = vMovementLimitMin;
			scrollY += (speed * -gameMemory->hero.direction.y);
		}
		else if ( heroPosition.y > vMovementLimitMax)
		{
			heroPosition.y = vMovementLimitMax;
			scrollY += (speed * -gameMemory->hero.direction.y);
		}
		else
		{
			heroPosition.y += speed * heroDirection.y;
		}

		// Find the tile the character is on
		int32 tileX = (heroPosition.x + halfSprite - scrollX) / SPRITE_SIZE;
		int32 tileY = (heroPosition.y + halfSprite - scrollY) / SPRITE_SIZE;
		uint32 hackTree = tileX + _mapSettings.width * tileY;

		if (map[tileX + _mapSettings.width * tileY] == TREE)
		{
			LogInfo("COLLISION@ %d:%dx%d", hackTree, tileX, tileY);
		}
		else
		{
			LogInfo("%dx%d",tileX, tileY);
		}
	}

	// Opaque objects first
	gameApi.spriteBatch.begin(gameMemory->material);
		drawMap(gameApi, _mapSettings, map, gameMemory->zoom ,scrollX, scrollY);
		gameApi.spriteBatch.submit(gameMemory->hero.sprite);
	gameApi.spriteBatch.end();
}

//---------------------------------------------------------------------------
// Game finalization
//---------------------------------------------------------------------------
void gameStop()
{
	LogInfo("Game stopped");
}

