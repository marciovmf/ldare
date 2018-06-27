

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

	void ldk_memory_freeHeap(Heap* heap)
	{
		free(heap);
	}
	
	void ldk_memory_set(void* memory, unsigned char value, size_t size)
	{
		// can we set 8 bytes at a time ?
		int32 qwordCount = size / 8;
		int64* dwordPtr = (int64*) memory;

		if (qwordCount > 0)
		{
			register uint64 qwordValue = value;

			if (value != 0)
			{
				qwordValue |= qwordValue << 8;
				qwordValue |= qwordValue << 16;
				qwordValue |= qwordValue << 32;
			}

			for (int i = 0; i < qwordCount; i++) 
			{
				*dwordPtr++ = qwordValue;
			}
		}

		// set remaining bytes
		uint32 byteCount = size - qwordCount * 8;
		char* bytePtr = (char*) dwordPtr;
		for (int i = 0; i < byteCount; i++) 
		{
			*bytePtr++ = value;
		}
	}
}
