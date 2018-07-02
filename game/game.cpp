#include <ldk/ldk.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

struct GameState
{
	bool initialized;
	ldk::Material material;
	ldk::Sprite sprite;
	ldk::Audio bgMusic;
} *gameState = nullptr;


void setupSprite();
void gameInit(void* memory)
{
	LogInfo("Game initialized");
	gameState = (GameState*)memory;

	if (!gameState->initialized)
	{
		gameState->initialized = true;
		ldk::render::spriteBatchInit();
		gameState->material = ldk::render::loadMaterial("./assets/sprite.cfg"); 
		ldk::loadAudio("assets/bgmusic.wav", &gameState->bgMusic);
	}
	else
	{
		setupSprite();
	}
}

void setupSprite()
{
	gameState->sprite.position = {SCREEN_WIDTH /2, SCREEN_HEIGHT/2, 0};
	gameState->sprite.color = { 1.0, 1.0, 1.0, 1.0 };
	gameState->sprite.width = gameState->sprite.height = 64;
	gameState->sprite.srcRect = {0,0,100,75};
	gameState->sprite.angle = 0;
}

void gameStart()
{
	LogInfo("Game started");
	setupSprite();
	//ldk::playAudio(&gameState->bgMusic);
}

float heading = 0;
float drag = 0.001f;
float maxSpeed = 10.0f;
float acceleration = 1.5f;
ldk::Vec3 force = {};
float t = 0;

void gameUpdate(float deltaTime)
{
	ldk::Sprite& sprite = gameState->sprite;
	ldk::Material& material = gameState->material;

	// steering
	if (ldk::input::getKey(LDK_KEY_A)) sprite.angle += 5 * deltaTime;
	if (ldk::input::getKey(LDK_KEY_D)) sprite.angle -= 5 * deltaTime;

	// thrusting
	if (ldk::input::getKey(LDK_KEY_W))
	{
			heading = sprite.angle;
			force.x += cos(RADIAN(90) + heading) * acceleration * deltaTime;
			force.y += sin(RADIAN(90) + heading) * acceleration * deltaTime;
			t = 0;
	}
	else
	{
		//drag
		t += drag * deltaTime;
		force.x = ldk::lerp(force.x, 0, t);
		force.y = ldk::lerp(force.y, 0, t);
	}

	float speed = force.magnitude();

	if (speed > maxSpeed)
	{
		speed = maxSpeed/speed;
		force.x *= speed * deltaTime;
		force.y *= speed * deltaTime;
	}

	sprite.position = sprite.position + force;

	// wrap around the screen
	if (sprite.position.x > SCREEN_WIDTH) sprite.position.x = 0;
	if (sprite.position.x < 0) sprite.position.x = SCREEN_WIDTH;
	if (sprite.position.y > SCREEN_HEIGHT) sprite.position.y = 0;
	if (sprite.position.y < 0) sprite.position.y = SCREEN_HEIGHT;
	
	ldk::render::spriteBatchBegin(material);
		ldk::render::spriteBatchSubmit(sprite);
	ldk::render::spriteBatchEnd();
}

void gameStop()
{
	LogInfo("Game stopped");
}
