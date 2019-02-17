
#ifndef _LDK_STRING_UTIL_
#define _LDK_STRING_UTIL_

namespace ldk
{

  uint32 strlen(const char* string)
  {
   const char* p = string;
   while(*p)
   {
     ++p;
   }

   return p - string;
  }

	uint32 stringToHash(const char* str)
	{
		uint32 stringLen = strlen((char*)str);
		uint32 hash = 0;

    for(; *str; ++str)
    {
        hash += *str;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
	}
}
#endif// _LDK_STRING_UTIL_
