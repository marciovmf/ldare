#ifndef _LDARE_GAME_
#define _LDARE_GAME_

#include "..\ldare.h"

namespace ldare
{
	//---------------------------------------------------------------------------
	// Runtime settings the game can return to the engine 
	// during initialization
	//---------------------------------------------------------------------------
	struct GameRuntimeSettings
	{
		int32 windowWidth;
		int32 windowHeight;
	};

	//---------------------------------------------------------------------------
	// Game Interface
	// This interface is used by the engine to communicate with the game
	//---------------------------------------------------------------------------
	class Game
	{
		public:
			virtual GameRuntimeSettings initialize()=0;
			virtual void startGame()=0;
			virtual void updateGame()=0;
			virtual ~Game(){}
	};
}

#endif // _LDARE_GAME_
