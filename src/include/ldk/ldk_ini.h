#ifndef _LDK_INI_H_
#define _LDK_INI_H_

#define LDK_MAX_IDENTIFIER_SIZE 63

namespace ldk
{
	enum VariantType
	{
		BOOL,
		INT,
		FLOAT,
		STRING,
		VEC3,
		VEC4
	};

	struct Variant
	{
		char8 key[LDK_MAX_IDENTIFIER_SIZE];
		uint32 size;
		VariantType type;
		int32 hash;
	};

	struct VariantSection
	{
		int32 hash;
		uint32 variantCount;
		uint32 totalSize; //total size of variant section, including this header
		char8 name[LDK_MAX_IDENTIFIER_SIZE];
	};

	struct VariantSectionRoot
	{
		uint32 sectionCount;	
	};
}

#endif// _LDK_INI_H_
