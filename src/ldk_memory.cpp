#include <cstdlib>

namespace	ldkEngine
{
  static const uint32 LDK_MEMORY_ALLOCATION_SIGNATURE = 0x4C444B4D;
  static Allocation* _allocationList = nullptr;
  static size_t _totalMemoryUsed = 0;

  void* memory_alloc(size_t size, Allocation::Tag tag)
  {
    const size_t totalSize = size + sizeof(Allocation);
    void* mem = malloc(totalSize);
    if(!mem) LogError("Failed to allocate memory.");

    Allocation* allocation = (Allocation*) mem;
    allocation->signature = LDK_MEMORY_ALLOCATION_SIGNATURE;
    allocation->tag = tag;
    allocation->size = totalSize;
    allocation->prev = nullptr;
    allocation->next = _allocationList;

    if (_allocationList)
    {
      _allocationList->prev = allocation;
    }

    _allocationList = allocation;
    _totalMemoryUsed += totalSize;
    void* userMemory = (void*) ++allocation;
    return userMemory;
  }

  void* memory_realloc(void* mem, size_t size)
  {
    Allocation* allocation = (Allocation*) mem;
    --allocation; // find the allocation header before the user memory block

    size_t totalSize = size + sizeof(Allocation);
    LDK_ASSERT(allocation->signature == LDK_MEMORY_ALLOCATION_SIGNATURE, "[MEMORY] Invalid LDK memory block.");

    if (allocation->size >= totalSize)
    {
      LogWarning("[MEMORY] Will not try to shrink an LDK memory block.");
      return mem;
    }

    _totalMemoryUsed -= allocation->size;
    _totalMemoryUsed += totalSize;

    void* newMem = realloc((void*)allocation, totalSize);
    allocation = (Allocation*) newMem;
    allocation->size = totalSize;

    // Fix other blocks pointing at us, since our address might have changed
    if (allocation->prev) allocation->prev->next = allocation;
    if (allocation->next) allocation->next->prev = allocation;

    void* userMemory = (void*) ++allocation;
    return userMemory;
  }

  void memory_free(void* mem)
  {
    Allocation* allocation = (Allocation*) mem;
    --allocation;

    LDK_ASSERT(allocation->signature == LDK_MEMORY_ALLOCATION_SIGNATURE, "[MEMORY] Invalid LDK memory block.");
    // Fix other blocks pointing at us, since our address might have changed
    if (allocation->prev) allocation->prev->next = allocation->next;
    if (allocation->next) allocation->next->prev = allocation->prev;


    if (_allocationList == mem)
      _allocationList = nullptr;

    _totalMemoryUsed -= allocation->size;
    free((void*)allocation);
  }

  void memory_tag(void* mem, Allocation::Tag tag)
  {
#ifdef _LDK_DEBUG_
    Allocation* allocation = (Allocation*) mem;
    --allocation;
    LDK_ASSERT(allocation->signature == LDK_MEMORY_ALLOCATION_SIGNATURE, "[MEMORY] Invalid LDK memory block.");
    allocation->tag = tag;
#endif // _LDK_DEBUG_
  }

  size_t memory_getTotalUsed()
  {
    return _totalMemoryUsed;
  }
}
