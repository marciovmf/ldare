#include <windows.h>

namespace ldare {
	namespace platform {

		void* memoryAlloc(size_t size)
		{
			void* mem =
				VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#ifdef DEBUG
			if (!mem) { LogError("Error allocation memory"); }
#endif
			return mem;
		}

	}	// platform namespace
} 	// ldare namespace
