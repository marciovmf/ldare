#include <ldare\game.h>

//TODO: This is a test! Remove it when we have a renderer
#define DECLARE_EXTERN_GL
#include "../src/ldare_core_gl.h"
#undef DECLARE_EXTERN_GL

class TestGame : public ldare::Game
{
	public:
		ldare::GameRuntimeSettings initialize() override
		{
			ldare::GameRuntimeSettings settings = {};
			settings.windowWidth = 800;
			settings.windowHeight = 600;

			return settings;
		}

		void startGame() override
		{
			LogInfo("Game started");
		}

		void updateGame() override
		{
			LogInfo("Game Update...");
			glClear(GL_COLOR_BUFFER_BIT);
		}
};

