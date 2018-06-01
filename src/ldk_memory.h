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
}
#endif		// _LDK_MEMORY_H_
