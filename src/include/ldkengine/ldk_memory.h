#ifndef _LDK_MEMORY_H_
#define _LDK_MEMORY_H_

#define KILOBYTE(value) (size_t) ((value) * 1024LL)
#define MEGABYTE(value) (size_t) (KILOBYTE(value) * 1024LL)
#define GIGABYTE(value) (size_t) (MEGABYTE(value) * 1024LL)

namespace ldkEngine
{
  struct Allocation
  {
    enum Tag
    {
      GENERAL = 0,
      MATERIAL,
      RENDERER,
      SPRITE_BATCH,
      BITMAP,
      AUDIO,
      FONT,
      MESH,

      NUM_TAGS
    };

    uint32 signature; // 'LDKM' 0x4C444B4D
    Tag tag;          // A tag that identifies this allocation purpose
    size_t size;      // size of allocation, including prefix and sufix
    Allocation* next; // next allocation in the list
    Allocation* prev; // previouse allocation in the list
  };

  void* memory_alloc(size_t size, Allocation::Tag tag = Allocation::Tag::GENERAL);
  void* memory_realloc(void* mem, size_t size);
  void memory_free(void* mem);
  void memory_tag(void* mem, Allocation::Tag tag);
  LDK_API size_t memory_getTotalUsed();
  LDK_API void memory_printReport();
}
#endif		// _LDK_MEMORY_H_
