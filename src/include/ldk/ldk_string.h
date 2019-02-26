#ifndef _LDK_STRING_UTIL_
#define _LDK_STRING_UTIL_

namespace ldk
{

  constexpr uint32 strlen(const char* string)
  {
   const char* p = string;
   while(*p)
   {
     ++p;
   }

   return p - string;
  }

	constexpr uint32 stringToHash(const char* str)
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

	inline bool isLetter(char c)
	{
		return (c >= 64 && c <= 90) || (c >= 97 && c <= 122);
	}

	inline bool isDigit(char c)
	{
		return c >= 48 && c <= 57;
	}


}
#endif// _LDK_STRING_UTIL_
