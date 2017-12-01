#ifndef __LDARE_ASSET__
#define __LDARE_ASSET__

#define BITMAP_FILE_HEADER_SIGNATURE 0x4D42

namespace ldare
{
	typedef uint32 Shader;

	struct Texture
	{
		uint32 id;
		uint32 width;
		uint32 height;
	};

	struct Material
	{
		ldare::Shader shader;
		ldare::Texture texture;
	};
	
	struct Audio
	{
		//TODO: Remove this when memory/asset manager is done
		uint32 id;
		void*  audioFileMemoryToRelease_;
		size_t audioMemorySize_;

	};

	struct Bitmap
	{
		uint32 width;
		uint32 height;
		uchar8 *pixels;
		//TODO: Remove this when memory/asset manager is done
		void*  bmpFileMemoryToRelease_;
		size_t bmpMemorySize_;
	};

#define LDARE_ASSET_TYPE_FONT 0x1

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
		uint32 magic; 					// 0x4C444146 'LDAF' ldare asset file
		uint16 major; 					// major version
		uint16 minor; 					// minor version
		uint32 numAssets; 			// number of assets in this file
	};

	struct FontAsset
	{
		uint32 numGliphs; 			// number of font gliphs
		uint32 rasterWidth; 	 	// width of font raster bitmap
		uint32 rasterHeight; 		// height of font raster bitmap
		uint32 gliphDataOffset; // offset to gliph list from this header
		uint32 rasterDataOffset;// offset to font bitmap from this header
		uint16 firstCodePoint;  // first character codepoint defined in the font
		uint16 lastCodePoint;  	// last character codepoint defined in the font
		uint16 defaultCodePoint;// default character to be substituted in the font
  	uint32 averageCharWidth;// average character width
  	uint32 MaxCharWidth; 		// max character width
	};

	struct FontGliphRect
	{
		uint32 x;
		uint32 y;
		uint32 w;
		uint32 h;
	};

	//TODO: this is for testing only. materials will be defined on a custom asset file on something similar
#define ASSET_API_LOAD_MATERIAL(name) \
	ldare::Material name(const char* vertexShader, const char* fragmentShader, const char* textureFile)

	typedef ASSET_API_LOAD_MATERIAL(loadMaterialFunc);

#define ASSET_API_LOAD_AUDIO(name) \
	 bool name(const char* file, ldare::Audio* audio)

	typedef ASSET_API_LOAD_AUDIO(loadAudioFunc);

#define ASSET_API_PLAY_AUDIO(name) \
	 void name(const ldare::Audio* audio)

	typedef ASSET_API_PLAY_AUDIO(playAudioFunc);

	struct AssetApi
	{
		loadMaterialFunc* loadMaterial;
		loadAudioFunc* loadAudio;
		playAudioFunc* playAudio;
	};

	bool loadBitmap(const char* file, ldare::Bitmap* bitmap);
	bool loadAudio(const char* file, ldare::Audio* audio);
	void playAudio(const ldare::Audio* audio);
	void freeAsset(void* memory, size_t size);
} // namespace ldare

#endif // __LDARE_ASSET__
