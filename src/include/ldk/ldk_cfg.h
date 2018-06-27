#ifndef _LDK_CFG_H_
#define _LDK_CFG_H_

namespace ldk
{
	struct VariantSectionRoot;
	struct VariantSection;

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
	LDK_API void config_dispose(VariantSectionRoot* root);
}

#endif// _LDK_CFG_H_
