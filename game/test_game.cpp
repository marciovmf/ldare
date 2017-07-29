#include <ldare/game.h>

static LDGameContext gameContext;

LDGameContext gameInit()
{
	gameContext.windowWidth = 800;
	gameContext.windowHeight = 600;
	return gameContext;
}

void gameStart()
{
	LogInfo("Game started");
}

void gameUpdate()
{
	LogInfo("Game updating");
}

void gameStop()
{
	LogInfo("Game stopped");
}

