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

	//TODO: this is for testing only. materials will be defined by JSON file on something similar
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
