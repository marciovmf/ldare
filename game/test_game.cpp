#include <ldare/game.h>
#include <stdio.h>

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

void gameUpdate(const Input& input)
{
	KeyState keyValue = input.keyboard[KBD_W];

	if(keyValue.state)
	{
		if (keyValue.thisFrame)
		{
			LogInfo("DOWN");
		}
		else
		{
			LogInfo("HOLDING");
		}
	}
	else 	
	{
		if(keyValue.thisFrame)
			LogInfo("UP");
	}

}

void gameStop()
{
	LogInfo("Game stopped");
}

