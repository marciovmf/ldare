#include <ldk/ldk.h>

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
	gameState->sprite.position = {0, 100, 1};
	gameState->sprite.color = { 1.0, 1.0, 1.0, 1.0 };
	gameState->sprite.width = 100;
	gameState->sprite.height = 100;
	gameState->sprite.srcRect = {0,0,100,75};
}

void gameStart()
{
	LogInfo("Game started");
	setupSprite();
	ldk::playAudio(&gameState->bgMusic);
}

const float speed = 100.0f;
void gameUpdate(float deltaTime)
{
	ldk::Sprite& sprite = gameState->sprite;
	ldk::Material& material = gameState->material;

	if (ldk::input::getKey(LDK_KEY_W))
		sprite.position.y += speed * deltaTime;

	if (ldk::input::getKey(LDK_KEY_S))
		sprite.position.y -= speed * deltaTime;

	if (ldk::input::getKey(LDK_KEY_A))
		sprite.position.x -= speed * deltaTime;

	if (ldk::input::getKey(LDK_KEY_D))
		sprite.position.x += speed * deltaTime;

	if (ldk::input::getKey(LDK_KEY_Q))
		sprite.angle -= 3 * deltaTime;

	if (ldk::input::getKey(LDK_KEY_E))
		sprite.angle += 3 * deltaTime;

	ldk::render::spriteBatchBegin(material);
		ldk::render::spriteBatchSubmit(sprite);
	ldk::render::spriteBatchEnd();
}

void gameStop()
{
	LogInfo("Game stopped");
}
