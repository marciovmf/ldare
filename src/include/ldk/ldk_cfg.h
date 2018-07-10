#ifndef _LDK_CFG_H_
#define _LDK_CFG_H_

#define LDK_CFG_MAX_IDENTIFIER_SIZE 63

namespace ldk
{

	enum VariantType
	{
		INT = 0,
		BOOL,
		FLOAT,
		STRING,
	};

	struct Variant
	{
		char key[LDK_CFG_MAX_IDENTIFIER_SIZE + 1];
		uint32 size;
		VariantType type;
		int32 hash;
		int32 arrayCount;
	};

	struct VariantSection
	{
		int32 hash;
		uint32 variantCount;
		uint32 totalSize; //total size of variant section, including this header
		char name[LDK_CFG_MAX_IDENTIFIER_SIZE + 1];
	};

	struct VariantSectionRoot
	{
		uint32 sectionCount;	
		size_t totalSize;
	};
	//struct VariantSectionRoot;
	//struct VariantSection;

	LDK_API VariantSectionRoot* config_parseFile(const char* fileName);
	LDK_API VariantSectionRoot* config_parseBuffer(void* buffer, size_t size);
	LDK_API VariantSection* config_getSection(VariantSectionRoot* rootSection, const char* name);
	LDK_API bool config_getInt(VariantSection* section, const char* key, int32* intValue);
	LDK_API bool config_getBool(VariantSection* section, const char* key, bool* boolValue);
	LDK_API bool config_getFloat(VariantSection* section, const char* key, float* floatValue);
	LDK_API const bool config_getString(VariantSection* section, const char* key, char** stringValue);
	LDK_API int32 config_getIntArray(VariantSection* section, const char* key, int32** array);
	LDK_API int32 config_getFloatArray(VariantSection* section, const char* key, float** array);
	LDK_API int32 config_getBoolArray(VariantSection* section, const char* key, bool** array);
	LDK_API int32 config_getStringArray(VariantSection* section, const char* key, char*** array);
	LDK_API VariantSection* config_getFirstSection(VariantSectionRoot* root);
	LDK_API VariantSection* config_getNextSection(VariantSectionRoot* root, VariantSection* section);
	LDK_API Variant* config_getFirstVariant(VariantSection* section);
	LDK_API Variant* config_getNextVariant(VariantSection* section, Variant* variant);
	LDK_API void config_dispose(VariantSectionRoot* root);
}

#endif// _LDK_CFG_H_
