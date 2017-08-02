#ifndef __LDARE_PLATFORM__
#define __LDARE_PLATFORM__

namespace ldare 
{
	namespace platform 
	{
		//---------------------------------------------------------------------------
		// Requests allocation of size bytes of memory
		//---------------------------------------------------------------------------
		void* memoryAlloc(size_t size);

	}
}
#endif	// __LDARE_PLATFORM__
