/**
 * win32_platform.h
 * Win32 implementation for ldare platform functions
 */

namespace ldare {
	namespace platform {

		void* memoryAlloc(size_t size)
		{
			void* mem =
				VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#ifdef DEBUG
			ASSERT(mem!=0);
#endif
			if (!mem) { LogError("Error allocaing memory"); }
			return mem;
		}

	}	// platform namespace
} 	// ldare namespace
