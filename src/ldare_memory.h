#ifndef __LDARE_MEMORY__
#define __LDARE_MEMORY__

namespace ldare
{
	namespace memory
	{
		struct Heap;
		struct HeapAllocationHeader
		{
			Heap* heap; 					// Pool that allocated this memory;
			HeapAllocationHeader* prev;
			HeapAllocationHeader* next;
		};

		struct Heap
		{
			size_t memorySize; 										// virtual memory size
			size_t objectSize; 										// size of objects this heap contais
			size_t totalUsed; 										// total amount of memory used
			HeapAllocationHeader* freeMemList; 		// start of free heap object list
			HeapAllocationHeader* usedMemList; 		// start of used heap object list
		};

		void foo();

		//---------------------------------------------------------------------------
		// Get memory from heap
		//---------------------------------------------------------------------------
		void* getHeapMemory(ldare::memory::Heap* heap);
		
		//---------------------------------------------------------------------------
		// Free heap memory
		//---------------------------------------------------------------------------
		void freeHeapMemory(void* memory);
	}
}
#endif // __LDARE_MEMORY__
