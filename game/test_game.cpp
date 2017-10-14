#include <ldare/ldare_game.h>
#include <stdio.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define FULLSCREEN 0

using namespace ldare;
static ldare::GameContext gameContext;

// sprite rects
static float spriteSize = 128.0f;
static Rectangle srcWall = {7 * spriteSize, 0, spriteSize, spriteSize};
static Rectangle srcWallBorder = {7 * spriteSize, 0, spriteSize, spriteSize};
static Rectangle srcTarget = { 11 * spriteSize ,  0 * spriteSize, spriteSize, spriteSize};
static Rectangle srcGround = { 11 * spriteSize, 1* spriteSize, spriteSize, spriteSize};
static Rectangle srcBox = { 1 * spriteSize, 7 * spriteSize, spriteSize, spriteSize};
static Rectangle srcHero = { 3 * spriteSize, 2 * spriteSize, spriteSize, spriteSize};

static struct Animation
{
	float elapsedTime;
	float totalTime;
	uint32 numFrames;
	uint32 currentFrameIndex;
	Rectangle* frames;
	float timePerFrame;
} _walkAnimationRight, _walkAnimationLeft, _walkAnimationUp, _walkAnimationDown;

static Rectangle _srcWalkRight[] = {
	{ 0 * spriteSize, 0 * spriteSize, spriteSize, spriteSize},
	{ 1 * spriteSize, 0 * spriteSize, spriteSize, spriteSize},
	{ 2 * spriteSize, 0 * spriteSize, spriteSize, spriteSize},
};

static Rectangle _srcWalkLeft[] = {
	{ 3 * spriteSize, 0 * spriteSize, spriteSize, spriteSize},
	{ 4 * spriteSize, 0 * spriteSize, spriteSize, spriteSize},
	{ 5 * spriteSize, 0 * spriteSize, spriteSize, spriteSize},
};

static Rectangle _srcWalkUp[] = {
	{ 3 * spriteSize, 2 * spriteSize, spriteSize, spriteSize},
	{ 4 * spriteSize, 2 * spriteSize, spriteSize, spriteSize},
	{ 5 * spriteSize, 2 * spriteSize, spriteSize, spriteSize},
};

static Rectangle _srcWalkDown[] = {
	{ 0 * spriteSize, 2 * spriteSize, spriteSize, spriteSize},
	{ 1 * spriteSize, 2 * spriteSize, spriteSize, spriteSize},
	{ 2 * spriteSize, 2 * spriteSize, spriteSize, spriteSize},
};

// hero animations
static struct GameLevel
{
	uint8* map;
	uint32 width;
	uint32 height;
	uint32 spriteSize;
	float tileWidth;
	float tileHeight;
} _gameLevel;

uint8 level1[] = 
{ 
	1,1,1,1,1,1,0,
	1,2,2,2,2,1,1,
	1,2,1,2,2,2,1,
	1,3,3,2,2,2,1,
	1,1,2,1,2,2,1,
	1,1,2,2,2,1,0,
	0,1,1,1,1,1,0
};

struct GameData
{
	Vec2 resolution;
	Sprite hero;
	Material material;
	float x=0;
	float y=0;
	float step;
	Sprite tiles[50];
	Sprite box;
} *gameMemory = nullptr;

void loadLevel(GameLevel& gameLevel, Sprite* sprites)
{
	float stepX = SCREEN_WIDTH/gameLevel.width;
	float stepY = SCREEN_HEIGHT/gameLevel.height;

	for (uint32 i=0; i < gameLevel.width; i++)
	{
		for (uint32 j=0; j < gameLevel.height; j++)
		{
			uint32 e = (uint32) ( i + j* gameLevel.width);
			Sprite& sprite = sprites[e];
			sprite.position = { stepX * i,  SCREEN_HEIGHT - stepY - (stepY * j), 0};
			sprite.width = stepX;
			sprite.height = stepY;
			switch (gameLevel.map[e])
			{
				case 1: sprite.srcRect = srcWall; break;
				case 2: sprite.srcRect = srcGround; break;
				case 3: sprite.srcRect = srcTarget; break;
				case 0:
				default:
								//sprite.srcRect = srcWall;
								sprite.width = 0;
								sprite.height = 0;
								break;
			}
		}
	}
}

//---------------------------------------------------------------------------
// Game Engine Initialization
//---------------------------------------------------------------------------
ldare::GameContext gameInit()
{
	gameContext.windowWidth = 640; 						// game window width
	gameContext.windowHeight = 480; 					// game window height
	gameContext.Resolution.width = SCREEN_WIDTH;
	gameContext.Resolution.height = SCREEN_HEIGHT;
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
		gameMemory->step = spriteSize * 1;
		gameMemory->x = gameMemory->y = 0;
		// load material
		gameMemory->material = gameApi.asset.loadMaterial(
				(const char*)"./assets/sprite.vert", 
				(const char*) "./assets/sprite.frag", 
				(const char*)"./assets/sokoban/tiles.bmp");
	}


	// Set up walk animation
	_walkAnimationRight.elapsedTime=0;
	_walkAnimationRight.numFrames = 3;
	_walkAnimationRight.totalTime = 0.3f;
	_walkAnimationRight.frames = _srcWalkRight;
	_walkAnimationRight.timePerFrame = _walkAnimationRight.totalTime / _walkAnimationRight.numFrames;

	_walkAnimationLeft = _walkAnimationRight;
	_walkAnimationUp = _walkAnimationRight;
	_walkAnimationDown = _walkAnimationRight;
	_walkAnimationLeft.frames = _srcWalkLeft;
	_walkAnimationUp.frames = _srcWalkUp;
	_walkAnimationDown.frames = _srcWalkDown;

	_gameLevel.map = level1;
	_gameLevel.width = 7;
	_gameLevel.height = 7;
	_gameLevel.spriteSize = spriteSize;
	loadLevel(_gameLevel, gameMemory->tiles);

	// Setup hero and box
	Sprite sprite;
	sprite.color = Vec3{0.0f, 0.0f, 1.0f};
	sprite.width = SCREEN_WIDTH/_gameLevel.width;
	sprite.height= SCREEN_HEIGHT/_gameLevel.height;
	sprite.srcRect = srcHero;
	sprite.position = Vec3{3 * 128.0f , 128.0f * 3,0.0f};
	gameMemory->hero = sprite;
	gameMemory->box = sprite;
	gameMemory->box.position = {5 * sprite.width, 2 * sprite.height, 0.0f };
	gameMemory->box.srcRect = srcBox;

}
//---------------------------------------------------------------------------
// draw level
//---------------------------------------------------------------------------
void drawLevel(GameApi& gameApi, Sprite* sprites, uint32 count)
{
	for(uint32 i =0; i < count; i++)
	{
		gameApi.spriteBatch.submit(gameMemory->material, sprites[i]);
	}
}
//---------------------------------------------------------------------------
// Start an animation
//---------------------------------------------------------------------------
Animation* startAnimation(Animation& animation)
{
		// force start of 1st frame
		animation.elapsedTime = animation.numFrames - 1;
		animation.currentFrameIndex = animation.timePerFrame;
		return &animation;
}
//---------------------------------------------------------------------------
// Updates an animation
//---------------------------------------------------------------------------
static inline Rectangle& updateAnimation(Animation& animation, float deltaTime)
{
		animation.elapsedTime += deltaTime;
		if (animation.elapsedTime > animation.timePerFrame)
		{
			animation.elapsedTime -= animation.timePerFrame;
			uint32 nextFrameIndex = (animation.currentFrameIndex + 1)  % animation.numFrames;
			LogInfo("Playing frame %d", nextFrameIndex);
			animation.currentFrameIndex = nextFrameIndex;
		}

	return animation.frames[animation.currentFrameIndex];
}

//---------------------------------------------------------------------------
// Game update
//---------------------------------------------------------------------------
Animation* currentAnimation = nullptr;
void gameUpdate(const float deltaTime, const Input& input, ldare::GameApi& gameApi)
{
	float& x = gameMemory->x;
	float& y = gameMemory->y;
	bool update = false;

	// start animation according to walk direction
	if ( input.keyboard[KBD_W].thisFrame && input.keyboard[KBD_W].state)	
	{	
		currentAnimation = startAnimation(_walkAnimationUp);
		update = true;
	}
	else if ( input.keyboard[KBD_S].thisFrame && input.keyboard[KBD_S].state)	
	{	
		currentAnimation = startAnimation(_walkAnimationDown);
		update = true;
	}
	
	if ( input.keyboard[KBD_A].thisFrame && input.keyboard[KBD_A].state)	
	{	
		currentAnimation = startAnimation(_walkAnimationLeft);
		update = true;
	}
	else if ( input.keyboard[KBD_D].thisFrame && input.keyboard[KBD_D].state)	
	{	
		currentAnimation = startAnimation(_walkAnimationRight);
		update = true;
	}

	// Update current animation according to walk direction
	if ( input.keyboard[KBD_W].state)	
	{
		y += gameMemory->step * deltaTime;
		update = true;
	}
	else if ( input.keyboard[KBD_S].state )
	{
		y -= gameMemory->step * deltaTime;
		update = true;
	}
	
	if ( input.keyboard[KBD_A].state )
	{
		x -= gameMemory->step * deltaTime;
		update = true;
	}
	else if ( input.keyboard[KBD_D].state )
	{
		x += gameMemory->step * deltaTime;
		update = true;
	}


	if (update)
	{
		if ( currentAnimation == nullptr) 
		{
			LogError("Animation should not be null here" );
			return;
		};
		gameMemory->hero.position.x = x;
		gameMemory->hero.position.y = y;
		gameMemory->hero.srcRect = updateAnimation(*currentAnimation, deltaTime);
	}
	else
	{
		if(currentAnimation !=nullptr)
		{
			currentAnimation->currentFrameIndex=0;
			currentAnimation->elapsedTime = 0;
		}
	}

	if (y > SCREEN_HEIGHT) y = 0;
	if (y < 0) y = SCREEN_HEIGHT;
	if (x > SCREEN_WIDTH) x = 0;
	if (x < 0) x = SCREEN_WIDTH;

	gameApi.spriteBatch.begin();
	drawLevel(gameApi, gameMemory->tiles, _gameLevel.width * _gameLevel.height);
	gameApi.spriteBatch.submit(gameMemory->material, gameMemory->hero);
	gameApi.spriteBatch.submit(gameMemory->material, gameMemory->box);
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

