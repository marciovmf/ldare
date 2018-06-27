#include <ldk/ldk.h>

ldk::Material material;
ldk::Sprite sprite;
ldk::Audio bgMusic;

void gameInit()
{
	LogInfo("Game initialized");
}

void gameStart()
{
	LogInfo("Game started");
	material = ldk::render::loadMaterial("./assets/sprite.cfg"); 
	ldk::loadAudio("assets/bgmusic.wav", &bgMusic);
	ldk::render::spriteBatchInit();
	sprite.position = {0, 100, 1};
	sprite.color = { 1.0, 1.0, 1.0, 1.0 };
	sprite.width = 100;
	sprite.height = 100;
	sprite.srcRect = {0,0,100,75};
	ldk::playAudio(&bgMusic);
}

const float speed = 100.0f;
void gameUpdate(float deltaTime)
{
	if (ldk::input::getKey(LDK_KEY_W))
		sprite.position.y += speed * deltaTime;

	if (ldk::input::getKey(LDK_KEY_S))
		sprite.position.y -= speed * deltaTime;

	if (ldk::input::getKey(LDK_KEY_A))
		sprite.position.x -= speed * deltaTime;

	if (ldk::input::getKey(LDK_KEY_D))
		sprite.position.x += speed * deltaTime;

	if (ldk::input::isKeyDown(LDK_KEY_J))
		sprite.srcRect.y -= speed * deltaTime;

	if (ldk::input::isKeyDown(LDK_KEY_K))
		sprite.srcRect.y += speed * deltaTime;

	ldk::render::spriteBatchBegin(material);
		ldk::render::spriteBatchSubmit(sprite);
	ldk::render::spriteBatchEnd();
}

void gameStop()
{
	LogInfo("Game stopped");
}

