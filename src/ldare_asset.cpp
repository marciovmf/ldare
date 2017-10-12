#ifdef _MSC_VER
#pragma pack(push,1)
#endif
struct BITMAP_FILE_HEADER
{
	uint16	FileType;					/* File type, always 4D42h ("BM") */
	uint32	FileSize;					//* Size of the file in bytes */
	uint16	Reserved1;				//* Always 0 */
	uint16	Reserved2;				//* Always 0 */
	uint32	BitmapOffset;			//* Starting position of image data in bytes */
	uint32	Size;							//* Size of this header in bytes */
	int32	Width;          		//* Image width in pixels */
	int32	Height;         		//* Image height in pixels */
	uint16  Planes;       		//* Number of color planes */
	uint16  BitsPerPixel; 		//* Number of bits per pixel */
	uint32	Compression;  		//* Compression methods used */
	uint32	SizeOfBitmap; 		//* Size of bitmap in bytes */
	int32	HorzResolution; 		//* Horizontal resolution in pixels per meter */
	int32	VertResolution; 		//* Vertical resolution in pixels per meter */
	uint32	ColorsUsed;   		//* Number of colors in the image */
	uint32	ColorsImportant;	//* Minimum number of important colors */
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

namespace ldare
{
	ldare::Material loadMaterial(const char* vertex, const char* fragment, const char* textureFile)
	{
		ldare::Material material;
		ldare::Bitmap bitmap;
		material.shader = ldare::loadShader(vertex, fragment);
		material.texture = ldare::loadTexture(textureFile);
		return material;
	}

	bool loadBitmap(const char* file, ldare::Bitmap* bitmap)
	{	
		bitmap->bmpFileMemroyToRelease_ = ldare::platform::loadFileToBuffer(file, &bitmap->bmpMemorySize_);
		if (!bitmap->bmpFileMemroyToRelease_ || bitmap->bmpMemorySize_ == 0) { return false; }

		BITMAP_FILE_HEADER *bitmapHeader = (BITMAP_FILE_HEADER*)bitmap->bmpFileMemroyToRelease_;

		//NOTE: compression info at https://msdn.microsoft.com/en-us/library/cc250415.aspx
		// it MUST be a valid 8bits per pixel, uncompressed bitmap
		if (bitmapHeader->FileType != BITMAP_FILE_HEADER_SIGNATURE
				|| (bitmapHeader->Compression != 0 && bitmapHeader->Compression != 3)
				|| bitmapHeader->BitsPerPixel != 32)
			return false;

		bitmap->pixels = (uint8*)(bitmapHeader)+bitmapHeader->BitmapOffset;
		bitmap->width = bitmapHeader->Width;
		bitmap->height = bitmapHeader->Height;

		//NOTE: Pixel format in memory is ABGR. Must rearrange it as RGBA to make OpenGL happy 
		//TODO: implement this with SIMD to make it faster
		uint32 numPixels = bitmap->width * bitmap->height;
		for (uint32 i = 0; i < numPixels; i++)
		{
			uint32 argb = *(((uint32*)bitmap->pixels) + i);
			uint32 a = (argb & 0x000000FF) << 24;
			uint32 r = (argb & 0xFF000000) >> 24;
			uint32 b = (argb & 0x0000FF00) << 8;
			uint32 g = (argb & 0x00FF0000) >> 8;
			*((uint32*)bitmap->pixels + i) = r | g | b | a;
		}

		return true;
	}

	void freeBitmap(ldare::Bitmap* bitmap)
	{
		ldare::platform::memoryFree(bitmap->bmpFileMemroyToRelease_, bitmap->bmpMemorySize_);
	}
} // namespace ldare
