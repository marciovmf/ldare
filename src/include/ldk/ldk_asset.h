#ifndef __LDK_ASSET_H__
#define __LDK_ASSET_H__

#define BITMAP_FILE_HEADER_SIGNATURE 0x4D42

namespace ldk
{
	struct Audio
	{
		//TODO: Remove this when memory/asset manager is done
		uint32 id;
	};

	struct Bitmap
	{
		uint32 width;
		uint32 height;
		uint32 bitsPerPixel;
		const uchar *pixels;
	};

#define LDK_ASSET_TYPE_FONT 0x1
/*
	struct AssetName
	{
		uint32 nameSize;
		uint32 nameOffset;
	};

	struct AssetEntry
	{
		uint32 type; 						// asset type
		uint32 dataOffset; 			// offset to data from start of file
		uint32 dataSize; 				// size of data
		uint32 assetNameId; 		// asset name id
	};

	struct AssetHeader
	{
		uint32 magic; 					// 0x4C444146 'LDAF' ldk asset file
		uint16 major; 					// major version
		uint16 minor; 					// minor version
		uint32 numAssets; 			// number of assets in this file
	};
*/
	struct FontGliphRect
	{
		uint32 x;
		uint32 y;
		uint32 w;
		uint32 h;
	};

	struct FontAsset
	{
		uint32 rasterWidth; 	 	// width of font raster bitmap
		uint32 rasterHeight; 		// height of font raster bitmap
		uint16 firstCodePoint;  // first character codepoint defined in the font
		uint16 lastCodePoint;  	// last character codepoint defined in the font
		uint16 defaultCodePoint;// default character to be substituted in the font
		FontGliphRect* gliphData;  // pointer to gliph data
		//TODO: add font kerning information here
	};
	
	LDK_API ldk::Bitmap* loadBitmap(const char* file);
  LDK_API ldk::Bitmap* getPlaceholderBmp();
	LDK_API ldk::Audio* loadAudio(const char* file);
	LDK_API ldk::MeshData* loadMesh(const char* file);
	LDK_API bool loadFont(const char* file, ldk::FontAsset** fontAsset);
	LDK_API void playAudio(const ldk::Audio* audio);
	LDK_API void freeAsset(void* memory);
} // namespace ldk

#endif // __LDK_ASSET_H__
