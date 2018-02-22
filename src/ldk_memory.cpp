namespace ldare
{
		//---------------------------------------------------------------------------
		// Get Memory from heap
		//---------------------------------------------------------------------------
		void* getMemory(Heap* heap)
		{
			// uninitialized heap huh?
			if ( heap->freeMemList == 0) 
			{
				return nullptr;
			}

			// get first free memory block			
			HeapAllocationHeader* allocationHeader = (HeapAllocationHeader*) heap->freeMemList;

			// where real memory really starts
			int32 headerSize = sizeof(HeapAllocationHeader);
			void* allocatedMemoryStart = ((int8*)allocationHeader) + headerSize;
			size_t totalAllocationSize = heap->objectSize + headerSize;

			// Are there enough space for new block on this heap ?
			size_t freeMemoryCount = heap->memorySize - heap->totalUsed;

			if (freeMemoryCount < totalAllocationSize)
			{
				// the free remaining free space - if any - is not enough, so
				// we just ignore it and link a new memory chunck
				LogWarning("Insuficient Heap space. Growing the heap now.");
				void* newMemoryBlock = ldare::platform::memoryAlloc(heap->memorySize);
				heap->freeMemList = (HeapAllocationHeader*) newMemoryBlock;
				heap->memorySize+=heap->memorySize;
				//TODO: include a member in HEAP for counting the total wasted memory ?
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

			// Insert at start of used list
			allocationHeader->prev = allocationHeader->next = 0;
			HeapAllocationHeader* firstUsedBlock = heap->usedMemList; 
			if( firstUsedBlock != 0 )
			{
				firstUsedBlock->prev = allocationHeader;
				allocationHeader->next = firstUsedBlock;
			}

			heap->totalUsed+=totalAllocationSize;
			heap->usedMemList = allocationHeader;

			memset(allocatedMemoryStart, 0xFF, heap->objectSize);
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

			ASSERT(allocationHeader->heap != 0, "Invalid allocation header");
			Heap* heap = allocationHeader->heap;

			// clear the memory
			memset(memory, 0, heap->objectSize);

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

			heap->totalUsed -= heap->objectSize + sizeof(HeapAllocationHeader);
			heap->freeMemList = allocationHeader;
		}
} //namespace ldare

