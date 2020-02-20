namespace ldk
{
  bool arena_resize(Arena* arena, size_t minimum)
  {
    uint32 newSize = (int32)(1.5f * arena->size);
    if (newSize < minimum)
    {
      LogInfo("resizing arena: old size = %d, new size = %d", arena->size, newSize);
      newSize += minimum;
    }

    arena->memory = platform::memoryRealloc(arena->memory, newSize);
    if (arena->memory)
    {
      arena->size = newSize;
    }
    else
    {
      return false;
    }
    return true;
  }

  bool arena_initialize(Arena* arena, uint32 initialSize, ldkEngine::Allocation::Tag tag)
  {
    arena->memory = ldkEngine::memory_alloc(initialSize, tag);
    if (arena->memory)
    {
      arena->size = initialSize;
      arena->used = 0;
      return true;
    }
    return false;
  }

  void arena_free(Arena* arena)
  {
    ldkEngine::memory_free((void*)arena);
  }
}
