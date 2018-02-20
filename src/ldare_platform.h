/**
 * ldare_platform.h
 * Defines platform specific functions for abstracting the real OS
 */
#ifndef __LDARE_PLATFORM__
#define __LDARE_PLATFORM__

namespace ldare 
{
	namespace platform 
	{
		struct GameModule;

		//---------------------------------------------------------------------------
		// Loads an entire file to memory
		//---------------------------------------------------------------------------
		void* loadFileToBuffer(const char* filename, size_t* bufferSize);

		//---------------------------------------------------------------------------
		// Requests allocation of size bytes of virtual memory
		//---------------------------------------------------------------------------
		void* memoryAlloc(size_t size);
	
		//---------------------------------------------------------------------------
		// releases allocated virtual memory
		//---------------------------------------------------------------------------
		void memoryFree(void* memory);
		
		//---------------------------------------------------------------------------
		// Get the number of ticks since engine initialization
		//---------------------------------------------------------------------------
		uint64 getTicks(); 

		//---------------------------------------------------------------------------
		// Get the time in seconds between 2 tick intervals
		//---------------------------------------------------------------------------
		float getTimeBetweenTicks(uint64 start, uint64 end); 

		//---------------------------------------------------------------------------
		// Loads the Game module.
		// Returns: true if game module is successfuly loaded
		//---------------------------------------------------------------------------
		bool loadGameModule(GameModule& gameModule);

		////---------------------------------------------------------------------------
		// Unloads the Game module.
		// Returns: true if game dll is successfuly unloaded
		//---------------------------------------------------------------------------
		bool unloadGameModule(GameModule& gameModule);

		//TODO: Add low level audio functions here
	}
}

#endif	// __LDARE_PLATFORM__
