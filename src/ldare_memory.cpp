#ifndef __LDARE_MEMORY__
#define __LDARE_MEMORY__

#include "ldare_platform.h"

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
			void* memory; 												// virtual memory
			size_t memorySize; 										// virtual memory size
			size_t objectSize; 										// size of objects this heap contais
			HeapAllocationHeader* freeMemList; 		// start of free heap object list
			HeapAllocationHeader* usedMemList; 		// start of used heap object list
		};

		//---------------------------------------------------------------------------
		// Get Memory from heap
		//---------------------------------------------------------------------------
		void* getMemory(Heap* heap)
		{
			// uninitialized heap huh?
			if ( heap->freeMemList == 0) 
			{
				heap->freeMemList = (HeapAllocationHeader*) heap->memory;
			}

			// get first free memory block			
			HeapAllocationHeader* allocationHeader = (HeapAllocationHeader*) heap->freeMemList;

			// where real memory really starts
			int32 headerSize = sizeof(HeapAllocationHeader);
			void* allocatedMemoryStart = ((int8*)allocationHeader) + headerSize;
			size_t totalAllocationSize = heap->objectSize + headerSize;

			// Are there enough space for new block on this heap ?
			if ((int8*)(allocationHeader) + totalAllocationSize > (int8*)(heap->memory) + heap->memorySize)
			{
				LogError("Could not allocate memory from heap");
				//TODO: allocate a new raw memory block
				//TODO:: Figure out how to join the new memory block
			}

			allocationHeader->heap = heap; 					// set this block's heap

			// remove from start of free list
			HeapAllocationHeader* nextFreeBlock = allocationHeader->next;

			if ( nextFreeBlock != 0)
			{
				nextFreeBlock->prev = 0;
			}
			else
			{
				// if this is the only block, just increment the memory
				nextFreeBlock = (HeapAllocationHeader*) ((int8*)heap->freeMemList + totalAllocationSize);
			}
			heap->freeMemList = nextFreeBlock;

			// Inset at start of used list
			allocationHeader->prev = allocationHeader->next = 0;
			HeapAllocationHeader* firstUsedBlock = heap->usedMemList; 
			if( firstUsedBlock != 0 )
			{
				firstUsedBlock->prev = allocationHeader;
				allocationHeader->next = firstUsedBlock;
			}
			heap->usedMemList = allocationHeader;
			return allocatedMemoryStart;
		}

		//---------------------------------------------------------------------------
		// Return memory to its heap
		//---------------------------------------------------------------------------
		void freeMemory(void* memory)
		{
			if (memory == 0) { return; }

			// get this blcok's header
			HeapAllocationHeader* allocationHeader = 
				(HeapAllocationHeader*)((int8*)memory - sizeof(HeapAllocationHeader));

			ASSERT(allocationHeader->heap != 0);
			Heap* heap = allocationHeader->heap;

			// fix neighbor connections
			HeapAllocationHeader* prev = allocationHeader->prev;
			HeapAllocationHeader* next = allocationHeader->next;

			if ( next != 0 )
			{
				next->prev = prev;
			}

			if ( prev !=0)
			{
				prev->next = next;
			}

			// place it at start of free block list
			allocationHeader->prev = 0;
			allocationHeader->next = heap->freeMemList;
			HeapAllocationHeader* firstFreeBlock =  heap->freeMemList;
			if (firstFreeBlock != 0)
			{
				firstFreeBlock->prev = allocationHeader;
			}
			//clear memory
			heap->freeMemList = allocationHeader;
		}

	} // namespace memory
} //namespace ldare

#endif 	// __LDARE_MEMORY__
