#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define GAME_RESOLUTION_WIDTH 1024
#define GAME_RESOLUTION_HEIGHT 576
#define FULLSCREEN 0

#define MAX_SHIP_SPEED 10
#define SHIP_SPEED_ACCELERATION 2
#define MAX_PROJECTILES 32
#define MAX_ASTEROIDS 32
#define WHITE {1.0, 1.0, 1.0, 1.0}
#include <ldk/ldk_game.h>
#include <wchar.h>
#include <stdio.h>

using namespace ldare;
#include "sprites.h"
#include "animation.cpp"

struct Entity
{	
	float dx, dy;		// entity delta x and y positions
	Sprite sprite;  // Current entity
	float timeout;  // entity gets deactivated if timeout is 0
	bool active;    // entity is ignored if not active
};

static Entity initEntity(float x, float y, Rectangle rect)
{
	Entity entity;
	entity.dx = entity.dy = 0;
	Sprite& sprite = entity.sprite;
	sprite.position = {x, y, 1};
	sprite.angle = 0;
	sprite.color = WHITE;
	sprite.width = rect.w/2;
	sprite.height = rect.h/2;
	sprite.srcRect = rect;
	return entity;
}

static void updateEntityPosition(Entity& entity)
{
	Sprite& sprite = entity.sprite;
	if (sprite.position.x > GAME_RESOLUTION_WIDTH)
		sprite.position.x = 0;	
	if (sprite.position.x < 0)
		sprite.position.x = GAME_RESOLUTION_WIDTH;

	if (sprite.position.y > GAME_RESOLUTION_HEIGHT)
		sprite.position.y = 0;	
	if (sprite.position.y < 0)
		sprite.position.y = GAME_RESOLUTION_HEIGHT;
}

static void updateShipPosition(Entity& entity, bool thrusting, float deltaTime)
{	
	Sprite& sprite = entity.sprite;
	
	float adjust = RADIAN(90);

	if ( thrusting)
	{
		entity.dx += cos(adjust + sprite.angle) * SHIP_SPEED_ACCELERATION * deltaTime ;
		entity.dy += sin(adjust + sprite.angle)  * SHIP_SPEED_ACCELERATION * deltaTime ;
	}
	else
	{
		// reduces the speed a little each frame
		entity.dx *= 0.995; 
		entity.dy *= 0.995;
	}

	if ( entity.dx > MAX_SHIP_SPEED )
		entity.dx = MAX_SHIP_SPEED;

	if ( entity.dy > MAX_SHIP_SPEED )
		entity.dy = MAX_SHIP_SPEED;

	sprite.position.x += entity.dx;
	sprite.position.y += entity.dy;

	updateEntityPosition(entity);
}

static ldare::GameContext gameContext;
// sprite rects
struct GameData
{
	Entity ship;
	Entity projectiles[MAX_PROJECTILES];
	Entity asteroids[MAX_ASTEROIDS];
	uint32 activeAsteroids;
	Material material;
	Vec2 resolution;
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
			(const char*)"./assets/sprites.bmp");

	Vec3 pos = {GAME_RESOLUTION_WIDTH/2, GAME_RESOLUTION_HEIGHT/2, 1};
	Vec4 color = {1.0, 1.0, 1.0, 1.0};

	gameMemory->ship = initEntity(pos.x, pos.y, playerShip1_red);
}

//---------------------------------------------------------------------------
// Game update
//---------------------------------------------------------------------------
float a =0;
void gameUpdate(const float deltaTime, const Input& input, ldare::GameApi& gameApi)
{
	bool thrusting = false;
	gameApi.text.flush();
	Entity& ship = gameMemory->ship;

	// thrust
	if (input.getKey(KBD_W))
	{
		thrusting = true;
	}

	// steering
	if (input.getKey(KBD_A))
	{
		ship.sprite.angle += RADIAN(180) * deltaTime;
	}
	else if (input.getKey(KBD_D))
	{
		ship.sprite.angle -= RADIAN(180) * deltaTime;
	}
	
	updateShipPosition(ship, thrusting, deltaTime);

	gameApi.spriteBatch.begin(gameMemory->material);
	gameApi.spriteBatch.submit(ship.sprite);
	gameApi.spriteBatch.end();
}

//---------------------------------------------------------------------------
// Game finalization
//---------------------------------------------------------------------------
void gameStop()
{
	LogInfo("Game stopped");
}

