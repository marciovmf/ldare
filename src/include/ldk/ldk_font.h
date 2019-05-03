#ifndef _LDK_FONT_H_
#define _LDK_FONT_H_

namespace ldk
{
  const uint32 LDK_MAX_FONT_NAME_LEN = 32;

  struct Bitmap;

  struct FontMetrics
  {
    uint16 firstCodePoint;  // first character codepoint defined in the font
    uint16 lastCodePoint;  	// last character codepoint defined in the font
    uint16 defaultCodePoint;// default character to be substituted in the font
    uint16 reserved;
  };

  struct FontData
  {
    char8 signature[4];     // 'FONT'
    char8 name[LDK_MAX_FONT_NAME_LEN];
    FontMetrics metrics;
    uint32 fontNameOffset;  // offset fron FontData start
    uint32 gliphDataOffset; // offset fron FontData start
  };

  struct Font
  {
    FontData fontData;
    char* name;
    ldk::Bitmap* bitmap;
    Rect* gliphs;
  };
}
#endif// _LDK_FONT_H_
