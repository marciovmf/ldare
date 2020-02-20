#ifndef _LDK_ARENA_H_
#define _LDK_ARENA_H_

#include <cstdlib>

namespace ldk
{
	struct Arena
	{
		void* memory;
		uint32 size;
		uint32 used;
	};

	bool arena_resize(Arena* anrea, size_t minimum);
  bool arena_initialize(Arena* arena, uint32 initialSize, ldkEngine::Allocation::Tag tag 
      = ldkEngine::Allocation::Tag::GENERAL);
	void arena_free(Arena* anrea);

}
#endif		// _LDK_ARENA_H_
