#ifndef _LDK_CFG_H_
#define _LDK_CFG_H_

#define LDK_CFG_MAX_IDENTIFIER_SIZE 63

namespace ldk
{

/// @defgroup Config Config
/// @details Functions for parsing and reading data from cfg files.
/// @{

  ////@brief Variant value type.
  enum VariantType
  {
    INT = 0,
    BOOL,
    FLOAT,
    STRING,
  };

  ///@brief Represents any value read from a cfg file.
  ///@details Variants are declared on configuration files as a key=value pair.
  /// A variant is declared inside a configuration Section.
  ///@see VariantSection
  struct Variant
  {
    char key[LDK_CFG_MAX_IDENTIFIER_SIZE + 1];
    uint32 size;
    VariantType type;
    int32 hash;
    int32 arrayCount;
  };

  ///@brief Represents a configuration section from a cfg file.
  ///@details A section is declared on configuration files between square
  ///brackets. A section named example, would be declared as [example] .
  struct VariantSection
  {
    int32 hash;
    uint32 variantCount;
    uint32 totalSize;///total size of variant section, including this header
    char name[LDK_CFG_MAX_IDENTIFIER_SIZE + 1];
  };

  ///@brief The first configuration section from a cfg file.
  ///@details The configuration root section.
  struct VariantSectionRoot
  {
    uint32 sectionCount;	
    size_t totalSize;
  };

  ///@brief Parses a config file by name.
  ///@param fileName - The name of the file to parse
  ///@returns the root variant section of the document.
  LDK_API const VariantSectionRoot* configParseFile(const char* fileName);

  ///@brief Parses a config buffer
  ///@param buffer - The buffer to parse
  ///@param size - size of the buffer
  ///@returns the root variant section of the document.
  LDK_API const VariantSectionRoot* configParseBuffer(const void* buffer, size_t size);

  ///@brief Gets a configuration section
  ///@param rootSection - The configuration root section
  ///@param name - The name of the section to get
  ///@returns the section identified by name.
  LDK_API const VariantSection* configGetSection(const VariantSectionRoot* rootSection, const char* name);

  ///@brief Gets an integer variant from a configuration section
  ///@param section - The section to get the variant from
  ///@param key - The name of the variant.
  ///@param value - A pointer to store the variant value if found
  ////@returns true if the variant is found
  LDK_API bool configGetInt(const VariantSection* section, const char* key, int32* value);

  ///@brief Gets an boolean variant from a configuration section
  ///@param section - The section to get the variant from
  ///@param key - The name of the variant.
  ///@param value - A pointer to store the variant value if found
  ////@returns true if the variant is found.
  LDK_API bool configGetBool(const VariantSection* section, const char* key, bool* value);

  ///@brief Gets an float variant from a configuration section
  ///@param section - The section to get the variant from
  ///@param key - The name of the variant.
  ///@param value - A pointer to store the variant value if found
  ////@returns true if the variant is found.
  LDK_API bool configGetFloat(const VariantSection* section, const char* key, float* value);

  ///@brief Gets a string variant from a configuration section
  ///@details The string variant value is a char*.
  ///@param section - The section to get the variant from
  ///@param key - The name of the variant.
  ///@param value - A pointer to store the variant string value.
  ///@returns true if the variant is found.
  LDK_API bool configGetString(const VariantSection* section, const char* key, char** value);

  ///@brief Gets an integer variant array from a configuration section
  ///@param section - The section to get the variant from
  ///@param key - The name of the variant.
  ///@param value - A pointer to store the variant array value.
  ///@returns true if the variant is found.
  LDK_API int32 configGetIntArray(const VariantSection* section, const char* key, int32** value);

  ///@brief Gets an float variant array from a configuration section
  ///@param section - The section to get the variant from
  ///@param key - The name of the variant.
  ///@param value - A pointer to store the variant array value.
  ///@returns true if the variant is found.
  LDK_API int32 configGetFloatArray(const VariantSection* section, const char* key, float** value);

  ///@brief Gets a bool variant array from a configuration section
  ///@param section - The section to get the variant from
  ///@param key - The name of the variant.
  ///@param value - A pointer to store the variant array value.
  ///@returns true if the variant is found.
  LDK_API int32 configGetBoolArray(const VariantSection* section, const char* key, bool** value);

  ///@brief Gets a string variant array from a configuration section
  ///@param section - The section to get the variant from
  ///@param key - The name of the variant.
  ///@param value - A pointer to store the variant array value.
  ///@returns true if the variant is found.
  LDK_API int32 configGetStringArray(const VariantSection* section, const char* key, char*** value);

  ///@brief Gets the first variant section
  ///@param root - The root variant section
  ///@returns A pointer to the first section or nullptr if no section is present.
  LDK_API const VariantSection* configGetFirstSection(const VariantSectionRoot* root);

  ///@brief Gets the configuration section after the current one.
  ///@param root - The root variant section
  ///@param section - The current variant section.
  ///@returns A pointer to the section or nullptr if no section is present.
  LDK_API const VariantSection* configGetNextSection(const VariantSectionRoot* root, const VariantSection* section);

  ///@brief Gets the first variant on a section.
  ///@param section - The current variant section.
  ///@returns A pointer to the variant or nullptr if no variant is presetn.
  LDK_API const Variant* configGetFirstVariant(const VariantSection* section);

  ///@brief Gets the next variant on a section.
  ///@param section - The current variant section.
  ///@param variant - The current variant.
  ///@returns A pointer to the variant or nullptr if no variant is presetn.
  LDK_API const Variant* configGetNextVariant(const VariantSection* section, const Variant* variant);

  ///@brief Disposes any resources allocated fot storing the configuration data.
  ///@param root - The configuration root section.
  LDK_API void configDispose(const VariantSectionRoot* root);

  ///@}
}

#endif// _LDK_CFG_H_
