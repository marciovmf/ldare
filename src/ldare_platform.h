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
		// Reads an entire file to memory. The necessary memory will be allocated.
		// Returns: pointer to file loaded in memory
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

		//---------------------------------------------------------------------------
		// Unloads the Game module.
		// Returns: true if game dll is successfuly unloaded
		//---------------------------------------------------------------------------
		bool unloadGameModule(GameModule& gameModule);

		//---------------------------------------------------------------------------
		// Plays the audio buffer identified by the passed audioBufferId
		//---------------------------------------------------------------------------
		void playAudio(uint32 audioBufferId);

		//---------------------------------------------------------------------------
		// Creates an audio buffer for the given audio data
		// Returns: id of the audio buffer
		//---------------------------------------------------------------------------
		uint32 createAudioBuffer(void* fmt, uint32 fmtSize, void* data, uint32 dataSize);
	}
}

#endif	// __LDARE_PLATFORM__
