//TODO(marcio): Review if this is really necessary or if all memory management will be done by the platform layer
#ifndef _LDK_MEMORY_H_
#define _LDK_MEMORY_H_

#define KILOBYTE(value) (size_t) ((value) * 1024LL)
#define MEGABYTE(value) (size_t) (KILOBYTE(value) * 1024LL)
#define GIGABYTE(value) (size_t) (MEGABYTE(value) * 1024LL)

#ifdef _LDK_DEBUG_		
#	define LDK_HEAP_INI_DATA 1
#	define LDK_HEAP_MATERIAL 2
#	define LDK_HEAP_SHADER 3
#endif

#include <cstdlib>

namespace ldk
{
	struct Heap
	{
		void* memory;
		uint32 size;
		uint32 used;
	};

	bool ldk_memory_resizeHeap(Heap* heap, size_t minimum);
	bool ldk_memory_allocHeap(Heap* heap, uint32 initialSize);
	void ldk_memory_freeHeap(Heap* heap);
	LDK_API void ldk_memory_set(void* memory, unsigned char value, size_t size);
}
#endif		// _LDK_MEMORY_H_
