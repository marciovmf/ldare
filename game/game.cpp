#include <ldk/ldk.h>

static ldk::Core* core;

void gameInit(ldk::Core* ldkCore)
{
	LogInfo("Game initialized");
	core = ldkCore;
}

void gameStart()
{
	LogInfo("Game started");
}

void gameUpdate(float deltaTime)
{
//	if (core->keyboard.getKeyDown(LDK_KEY_A))
//		LogInfo("A pressed");	
//	else if (core->keyboard.getKeyUp(LDK_KEY_A))
//		LogInfo("A released");

}

void gameStop()
{
	LogInfo("Game stopped");
}
