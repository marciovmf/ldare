#ifndef _LDK_MEMORY_H_
#define _LDK_MEMORY_H_

#define KILOBYTE(value) (size_t) ((value) * 1024LL)
#define MEGABYTE(value) (size_t) (KILOBYTE(value) * 1024LL)
#define GIGABYTE(value) (size_t) (MEGABYTE(value) * 1024LL)


namespace ldare
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

		//---------------------------------------------------------------------------
		// Get memory from heap
		//---------------------------------------------------------------------------
		void* getHeapMemory(ldare::Heap* heap);
		
		//---------------------------------------------------------------------------
		// Free heap memory
		//---------------------------------------------------------------------------
		void freeHeapMemory(void* memory);
}
#endif		// _LDK_MEMORY_H_
