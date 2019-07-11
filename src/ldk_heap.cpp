namespace ldk
{
  bool heap_resize(Heap* heap, size_t minimum)
  {
    uint32 newSize = 2 * heap->size;
    if (newSize < minimum)
      newSize += minimum;

    heap->memory = platform::memoryRealloc(heap->memory, newSize);
    if (heap->memory)
    {
      heap->size = newSize;
    }
    else
    {
      return false;
    }
    return true;
  }

  bool heap_alloc(Heap* heap, uint32 initialSize)
  {
    heap->memory = ldkEngine::memory_alloc(initialSize, ldkEngine::Allocation::Tag::GENERAL);
    if (heap->memory)
    {
      heap->size = initialSize;
      heap->used = 0;
      return true;
    }
    return false;
  }

  void heap_free(Heap* heap)
  {
    ldkEngine::memory_free((void*)heap);
  }
}
