#include <ldk/ldk.h>

void gameInit()
{
	LogInfo("Game initialized");
}

void gameStart()
{
	LogInfo("Game started");
}

void gameUpdate(float deltaTime)
{
	if (ldk::input::isKeyDown(LDK_KEY_A))
		LogInfo("A pressed");	
	else if (ldk::input::isKeyUp(LDK_KEY_A))
		LogInfo("A released");
}

void gameStop()
{
	LogInfo("Game stopped");
}
