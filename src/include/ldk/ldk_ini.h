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
		char8 key[LDK_MAX_IDENTIFIER_SIZE + 1];
		uint32 size;
		VariantType type;
		int32 hash;
	};

	struct VariantSection
	{
		int32 hash;
		uint32 variantCount;
		uint32 totalSize; //total size of variant section, including this header
		char8 name[LDK_MAX_IDENTIFIER_SIZE + 1];
	};

	struct VariantSectionRoot
	{
		uint32 sectionCount;	
	};

	VariantSectionRoot* ldk_config_parseFile(const char8* fileName);
	VariantSectionRoot* ldk_config_parseBuffer(const char8* fileName);
	VariantSection* ldk_config_getSection(VariantSectionRoot* rootSection, const char* name);
	Variant* ldk_config_getVariant(const VariantSection* section, const char* key);
	bool ldk_config_getInt(VariantSection* section, const char* key, int32* intValue);
	bool ldk_config_getBool(VariantSection* section, const char* key, bool* boolValue);
	bool ldk_config_getFloat(VariantSection* section, const char* key, float* floatValue);
	const bool ldk_config_getString(VariantSection* section, const char* key, char** stringValue);
}

#endif// _LDK_INI_H_
