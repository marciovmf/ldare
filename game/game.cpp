#include <ldk/ldk.h>

ldk::Material material;
ldk::Sprite sprite;

void gameInit()
{
	LogInfo("Game initialized");
}

void gameStart()
{
	LogInfo("Game started");
	material = ldk::render::loadMaterial(
			(const char8*)"./assets/sprite.vert",
			(const char8*) "./assets/sprite.frag", 
			(const char8*)"./assets/sprites.bmp");

	ldk::render::spriteBatchInit();
	sprite.position = {100, 100, 1};
	sprite.color = {1.0, 0, 0, 1.0};
	sprite.width = sprite.height = 100;
	sprite.srcRect = {0,0,100,100};
}

void gameUpdate(float deltaTime)
{
	if (ldk::input::isKeyDown(LDK_KEY_A))
		LogInfo("A pressed");	
	else if (ldk::input::isKeyUp(LDK_KEY_A))
		LogInfo("A released");

	ldk::render::spriteBatchBegin(material);
		ldk::render::spriteBatchSubmit(sprite);
	ldk::render::spriteBatchEnd();
	ldk::render::spriteBatchFlush();
}

void gameStop()
{
	LogInfo("Game stopped");
}

