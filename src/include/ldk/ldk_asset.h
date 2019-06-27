#ifndef _LDK_ASSET_H_
#define _LDK_ASSET_H_

namespace ldk
{
	struct Audio
	{
		uint32 id;
	};

	struct Bitmap
	{
		uint32 width;
		uint32 height;
		uint32 bitsPerPixel;
		const uchar *pixels;
	};

  struct FontGliphRect
	{
		uint32 x;
		uint32 y;
		uint32 w;
		uint32 h;
	};

  const uint32 LDK_MAX_FONT_NAME = 32;
  struct FontInfo
  {
    char8 name[LDK_MAX_FONT_NAME];
    uint32 fontSize;
		uint16 firstCodePoint;  // first character codepoint defined in the font
		uint16 lastCodePoint;  	// last character codepoint defined in the font
		uint16 defaultCodePoint;// default character to be substituted in the font
  };

	struct FontData
	{
    FontInfo info;
		uint32 rasterWidth; 	 	// width of font raster bitmap
		uint32 rasterHeight; 		// height of font raster bitmap
		//TODO: add font kerning information here
	};

  struct Font
  {
    FontInfo* fontInfo;
    FontGliphRect* gliphData;
  };

	LDK_API ldk::Handle loadBitmap(const char* file);
	LDK_API ldk::Handle loadAudio(const char* file);
	LDK_API ldk::Handle loadFont(const char* file);
	LDK_API void playAudio(ldk::Handle audioHandle);
	LDK_API void unloadAsset(Handle handle);
} // namespace ldk

#endif // _LDK_ASSET_H_
