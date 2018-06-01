namespace	ldk
{
	bool ldk_memory_resizeHeap(Heap* heap, size_t minimum)
	{
		uint32 newSize = 2 * heap->size;
		 if (newSize < minimum)
			 newSize += minimum;

		heap->memory = realloc(heap->memory, newSize);
		if (heap->memory)
		{
			heap->size = newSize;
		}
		else
		{
			LogError("Could not reallocate memory for parsed ini data");
			return false;
		}
		return true;
	}

	bool ldk_memory_allocHeap(Heap* heap, uint32 initialSize)
	{
		heap->memory = malloc(initialSize);
		if (heap->memory)
		{
			heap->size = initialSize;
			heap->used = 0;
			return true;
		}
		return false;
	}
}
