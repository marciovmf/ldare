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

		typedef void* DynamicLibrary;

		//---------------------------------------------------------------------------
		// Requests allocation of size bytes of memory
		//---------------------------------------------------------------------------
		void* memoryAlloc(size_t size);

	}
}

#endif	// __LDARE_PLATFORM__
