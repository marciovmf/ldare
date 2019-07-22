#include <cstdlib>

namespace	ldkEngine
{
  static const uint32 LDK_MEMORY_ALLOCATION_SIGNATURE = 0x4C444B4D;
  static Allocation* _allocationList = nullptr;
  static size_t _totalMemoryUsed = 0;
  static size_t _peakMemoryUsed = 0;
  static uint32 _allocationCount = 0;

  static inline void _updateTotalMemory(size_t size)
  {
    _totalMemoryUsed += size;
    if(_peakMemoryUsed < _totalMemoryUsed)
      _peakMemoryUsed = _totalMemoryUsed;
  }

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

    LDK_ASSERT(allocation->signature == LDK_MEMORY_ALLOCATION_SIGNATURE, "[MEMORY] Invalid LDK memory block.");
    LDK_ASSERT(allocation->tag < Allocation::Tag::NUM_TAGS && allocation->tag >= 0, "[MEMORY] Invalid memory tag");

    _updateTotalMemory(totalSize);
    _allocationCount++;
   
    // user memory starts after the allocation header
    void* userMemory = (void*) ++allocation;
    return userMemory;
  }

  void* memory_realloc(void* mem, size_t size)
  {
    Allocation* allocation = (Allocation*) mem;
    --allocation; // find the allocation header before the user memory block

    size_t totalSize = size + sizeof(Allocation);
    LDK_ASSERT(allocation->signature == LDK_MEMORY_ALLOCATION_SIGNATURE, "[MEMORY] Invalid LDK memory block.");
    LDK_ASSERT(allocation->tag < Allocation::Tag::NUM_TAGS && allocation->tag >= 0, "[MEMORY] Invalid memory tag");

    if (allocation->size >= totalSize)
    {
      LogWarning("[MEMORY] Will not try to shrink an LDK memory block.");
      return mem;
    }

    _updateTotalMemory(totalSize - allocation->size);

    void* newMem = realloc((void*)allocation, totalSize);
    Allocation* newAllocation = (Allocation*) newMem;
    newAllocation->size = totalSize;
    LDK_ASSERT(newAllocation->tag < Allocation::Tag::NUM_TAGS && newAllocation->tag >= 0, "[MEMORY] Invalid memory tag");

    // Fix other blocks pointing at us, since our address might have changed
    if (_allocationList == allocation)
      _allocationList = newAllocation;

    if (newAllocation->prev)
      newAllocation->prev->next = newAllocation;

    if (newAllocation->next)
      newAllocation->next->prev = newAllocation;

    // user memory starts after the allocation header
    void* userMemory = (void*) ++newAllocation;
    return userMemory;
  }

  void memory_free(void* mem)
  {
    Allocation* allocation = (Allocation*) mem;
    --allocation;

    LDK_ASSERT(allocation->tag < Allocation::Tag::NUM_TAGS && allocation->tag >= 0, "[MEMORY] Invalid memory tag");
    LDK_ASSERT(allocation->signature == LDK_MEMORY_ALLOCATION_SIGNATURE, "[MEMORY] Invalid LDK memory block.");
    
    // Fix other blocks pointing at us, since our address might have changed
    if (_allocationList == allocation)
      _allocationList = allocation->next;

    if (allocation->prev)
      allocation->prev->next = allocation->next;

    if (allocation->next)
      allocation->next->prev = allocation->prev;

    _updateTotalMemory(-allocation->size);
    _allocationCount--;
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

    LDK_ASSERT(allocation->tag < Allocation::Tag::NUM_TAGS && allocation->tag >= 0, "[MEMORY] Invalid memory tag");
  }

  size_t memory_getTotalUsed()
  {
    return _totalMemoryUsed;
  }

  void memory_printReport()
  {
    size_t tagMemory[Allocation::Tag::NUM_TAGS] = {};

    Allocation* allocation = _allocationList;
    while(allocation != nullptr)
    {
      tagMemory[allocation->tag] += allocation->size;
      allocation = allocation->next;
    }

    LogInfo("[MEMORY] Peak memory usage: %zuk.\tCurrent memory usage: %zuk.\n\tGENERAL %zuk.\n\tMATERIAL %zuk.\n\tRENDERER %zuk.\n\tSPRITE_BATCH %zuk.\n\tBITMAP %zuk.\n\tAUDIO %zuk.\n\tFONT %zuk.\n\tMESH %zuk.",
        _peakMemoryUsed / 1024,
        _totalMemoryUsed / 1024,
        tagMemory[(uint32)Allocation::GENERAL]/1024,
        tagMemory[(uint32)Allocation::MATERIAL]/1024,
        tagMemory[(uint32)Allocation::RENDERER]/1024,
        tagMemory[(uint32)Allocation::SPRITE_BATCH]/1024,
        tagMemory[(uint32)Allocation::BITMAP]/1024,
        tagMemory[(uint32)Allocation::AUDIO]/1024,
        tagMemory[(uint32)Allocation::FONT]/1024,
        tagMemory[(uint32)Allocation::MESH]/1024);
  }
}
