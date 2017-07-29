#ifndef _LDARE_GAME_
#define _LDARE_GAME_

#include <ldare/ldare.h>

	//---------------------------------------------------------------------------
	// Runtime settings the game can return to the engine 
	// during initialization
	//---------------------------------------------------------------------------
	struct LDGameContext
	{
		int32 windowWidth;
		int32 windowHeight;
	};

	//---------------------------------------------------------------------------
	// Game Interface
	// This interface is used by the engine to communicate with the game
	//---------------------------------------------------------------------------
	LDGameContext gameInit();
	void gameStart();
	void gameUpdate();
	void gameStop();

#endif // _LDARE_GAME_
