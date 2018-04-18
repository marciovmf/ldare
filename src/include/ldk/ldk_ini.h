#ifndef _LDK_INI_H_
#define _LDK_INI_H_

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

	struct StringLiteral
	{
		char8* start;
		uint32 lenght;
	};

	struct Variant
	{
		VariantType type;
		union 
		{
			uint8 boolValue;
			Vec3 vec3Value;
			uint32 intValue;
			float floatValue;
			Vec4 vec4Value;
			StringLiteral stringValue;
		};
	};

	
	struct ConfigSection;

	//Gets a configuration section
	const *ConfigSection ldk_cfg_getSection(const char8* name);

	//
	bool ldk_cfg_tryGetBool(const *ConfigSection, const char8* name, bool *value);
	bool ldk_cfg_tryGetInt(const *ConfigSection, const char8* name, uint32 *value);
	bool ldk_cfg_tryGetFloat(const *ConfigSection, const char8* name, float *value);
	bool ldk_cfg_tryGetVec3(const *ConfigSection, const char8* name, Vec3 *value);
	bool ldk_cfg_tryGetVec4(const *ConfigSection, const char8* name, Vec4 *value);
	bool ldk_cfg_tryGetString(const *ConfigSection, const char8* name, const char8* *value);

	// returns a list of variants. if count is not null, it is filled with the number of variants in the given section
	 const Variant* ldk_cfg_getVariantList(const *ConfigSection, uint32* count);
}
#endif// _LDK_INI_H_
