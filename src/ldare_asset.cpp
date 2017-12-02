
#define RIFF_FORMAT_WAVE 0x45564157
#define RIFF_FOURCC_RIFF 0x46464952
#define RIFF_FOURCC_FMT 0x20746d66
#define RIFF_FOURCC_DATA 0x61746164
namespace ldare
{

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
	//---------------------------------------------------------------------------
	// BMP file format specifics
	//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
// WAV file format specifics
//---------------------------------------------------------------------------
	struct RIFFAudioHeaderChunk
	{
		uint32 signature;
		uint32 chunkSize;
		uint32 chunkType;
	};

	struct RIFFAudioChunk
	{
		uint32 signature;
		uint32 chunkSize;
	};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

	//---------------------------------------------------------------------------
	// Material functions
	//---------------------------------------------------------------------------
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
		bitmap->bmpFileMemoryToRelease_ = ldare::platform::loadFileToBuffer(file, &bitmap->bmpMemorySize_);
		if (!bitmap->bmpFileMemoryToRelease_ || bitmap->bmpMemorySize_ == 0) { return false; }

		BITMAP_FILE_HEADER *bitmapHeader = (BITMAP_FILE_HEADER*)bitmap->bmpFileMemoryToRelease_;

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
			uint32 r = (argb & 0xFF000000) >> 24;
			uint32 g = (argb & 0x00FF0000) >> 8;
			uint32 b = (argb & 0x0000FF00) << 8;
			uint32 a = (argb & 0x000000FF) << 24;
			*((uint32*)bitmap->pixels + i) = r | g | b | a;
		}
		return true;
	}

	bool loadFont(const char* file, ldare::FontAsset** font)
	{
		size_t fontAssetSize;
		ldare::FontAsset* fontAsset = (ldare::FontAsset*) ldare::platform::loadFileToBuffer(file, &fontAssetSize);
		if (!fontAsset || fontAssetSize == 0) { return false; }

		// fix gliph array pointer
		fontAsset->gliphData = (FontGliphRect*)(((uint32)fontAsset) + (uint32)fontAsset->gliphData);
		*font = fontAsset;
		return true;
	}

	void freeAsset(void* memory, size_t size)
	{
		ldare::platform::memoryFree(memory, size);
	}

	//---------------------------------------------------------------------------
	// Audio functions
	//---------------------------------------------------------------------------
	static void* findAudioChunk(void* riffFileData, uint32 riffDataSize, uint32 fourcc, uint32* outSize)
	{
		*outSize = 0;
		uint8* data = (uint8*) riffFileData;
		uint8* endOfBuffer = data + riffDataSize;

		while ( data < endOfBuffer)
		{
			RIFFAudioChunk *chunk = (RIFFAudioChunk*) data;
			if ( chunk->signature == fourcc)
			{
				*outSize = chunk->chunkSize;
				return data + sizeof(RIFFAudioChunk);
			}
			data += chunk->chunkSize + sizeof(RIFFAudioChunk);
		}
		return nullptr;
	}

	bool loadAudio(const char* file, ldare::Audio* audio)
	{
		audio->audioFileMemoryToRelease_ = ldare::platform::loadFileToBuffer(file, (size_t*) &audio->audioMemorySize_);

		if (! audio->audioFileMemoryToRelease_ || audio->audioMemorySize_ == 0)
			return false; 

		RIFFAudioHeaderChunk* riffHeader = (RIFFAudioHeaderChunk*) audio->audioFileMemoryToRelease_;
		void* riffData = ((uint8*)audio->audioFileMemoryToRelease_ + sizeof(RIFFAudioHeaderChunk));

		if (riffHeader->signature != RIFF_FOURCC_RIFF || riffHeader->chunkType != RIFF_FORMAT_WAVE)
		{
			LogError("Invalid wave file");
			freeAsset(audio->audioFileMemoryToRelease_, audio->audioMemorySize_);
		}

		// find 'fmt' chunk
		uint32 fmtSize;
		void* fmt = findAudioChunk(riffData, riffHeader->chunkSize, RIFF_FOURCC_FMT, &fmtSize);
		if ( fmt == nullptr) 
		{
			LogError("Error loading wave format table");
			return false;
		}

		// find 'data' chunk
		uint32 dataSize;
		void* data = findAudioChunk(riffData, riffHeader->chunkSize, RIFF_FOURCC_DATA, &dataSize);
		if ( data == nullptr) 
		{
			LogError("Error loading wave data table");
			return false;
		}

		audio->id = ldare::platform::createAudioBuffer(fmt, fmtSize, data, dataSize);
		return true;
	}

	void playAudio(const ldare::Audio* audio)
	{
		ldare::platform::playAudio(audio->id);
	}

} // namespace ldare
