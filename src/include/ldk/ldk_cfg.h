#ifndef _LDK_CFG_H_
#define _LDK_CFG_H_

namespace ldk
{
	struct VariantSectionRoot;
	struct VariantSection;

	LDK_API VariantSectionRoot* ldk_config_parseFile(const char8* fileName);
	LDK_API VariantSectionRoot* ldk_config_parseBuffer(void* buffer, size_t size);
	LDK_API VariantSection* ldk_config_getSection(VariantSectionRoot* rootSection, const char* name);
	LDK_API bool ldk_config_getInt(VariantSection* section, const char* key, int32* intValue);
	LDK_API bool ldk_config_getBool(VariantSection* section, const char* key, bool* boolValue);
	LDK_API bool ldk_config_getFloat(VariantSection* section, const char* key, float* floatValue);
	LDK_API const bool ldk_config_getString(VariantSection* section, const char* key, char** stringValue);
}

#endif// _LDK_CFG_H_
