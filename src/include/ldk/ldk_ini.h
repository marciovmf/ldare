#ifndef _LDK_INI_H_
#define _LDK_INI_H_

#define LDK_MAX_IDENTIFIER_SIZE 63

namespace ldk
{
	struct VariantSectionRoot;
	struct VariantSection;

	VariantSectionRoot* ldk_config_parseFile(const char8* fileName);
	VariantSectionRoot* ldk_config_parseBuffer(void* buffer, size_t size);
	VariantSection* ldk_config_getSection(VariantSectionRoot* rootSection, const char* name);
	bool ldk_config_getInt(VariantSection* section, const char* key, int32* intValue);
	bool ldk_config_getBool(VariantSection* section, const char* key, bool* boolValue);
	bool ldk_config_getFloat(VariantSection* section, const char* key, float* floatValue);
	const bool ldk_config_getString(VariantSection* section, const char* key, char** stringValue);
}

#endif// _LDK_INI_H_
