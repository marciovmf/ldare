#ifndef __LDARE_ASSET__
#define __LDARE_ASSET__

#define BITMAP_FILE_HEADER_SIGNATURE 0x4D42

namespace ldare
{
	struct Material;
	struct Bitmap
	{
		uint32 width;
		uint32 height;
		uchar8 *pixels;
		//TODO: Remove this when memory/asset manager is done
		void*  bmpFileMemroyToRelease_;
		size_t bmpMemorySize_;
	};
	//TODO: this is for testing only. materials will be defined by JSON file on something similar
#define ASSET_API_LOAD_MATERIAL(name) \
	ldare::Material name(const char* vertexShader, const char* fragmentShader, const char* textureFile)

	typedef ASSET_API_LOAD_MATERIAL(loadMaterialFunc);

	struct AssetApi
	{
		loadMaterialFunc* loadMaterial;
	};

	bool loadBitmap(const char* file, ldare::Bitmap* bitmap);
	void freeBitmap(ldare::Bitmap* bitmap);
} // namespace ldare

#endif // __LDARE_ASSET__
