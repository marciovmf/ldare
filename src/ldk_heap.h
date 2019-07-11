#ifndef _LDK_HEAP_H_
#define _LDK_HEAP_H_

#include <cstdlib>

namespace ldk
{
	struct Heap
	{
		void* memory;
		uint32 size;
		uint32 used;
	};

	bool heap_resize(Heap* heap, size_t minimum);
	bool heap_alloc(Heap* heap, uint32 initialSize);
	void heap_free(Heap* heap);
}
#endif		// _LDK_HEAP_H_
