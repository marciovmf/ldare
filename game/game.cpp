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
	sprite.position = {0, 100, 1};
	sprite.color = { 1.0, 1.0, 1.0, 1.0 };
	sprite.width = 100;
	sprite.height = 100;
	sprite.srcRect = {0,0,100,75};
}

void gameUpdate(float deltaTime)
{
	if (ldk::input::getKey(LDK_KEY_W))
		sprite.position.y +=0.1;

	if (ldk::input::getKey(LDK_KEY_S))
		sprite.position.y -=0.1;

	if (ldk::input::getKey(LDK_KEY_A))
		sprite.position.x -=0.1;

	if (ldk::input::getKey(LDK_KEY_D))
		sprite.position.x +=0.1;

	if (ldk::input::isKeyDown(LDK_KEY_J))
	{
		sprite.srcRect.y -=100;
	}

	if (ldk::input::isKeyDown(LDK_KEY_K))
	{
		sprite.srcRect.y +=100;
	}

	ldk::render::spriteBatchBegin(material);
		ldk::render::spriteBatchSubmit(sprite);
	ldk::render::spriteBatchEnd();
}

void gameStop()
{
	LogInfo("Game stopped");
}

