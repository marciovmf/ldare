#include "ldk_wav.h"
#include "ldk_bitmap.h"
#include  <ldkengine/ldk_asset_internal.h>

// Renderer dependency
namespace ldk
{
  namespace renderer
  {
    //TODO(marcio): It would be nice to separate .mat file parsing from material creation
    ldk::Handle createMaterial(const char* file);
  }
}

namespace ldk
{
  static int32 _placeholderBmpData = 0xFFFF00FF;
  static ldk::Bitmap _placeholderBmp = {};

  ldk::Bitmap* getPlaceholderBmp()
  {
    if (_placeholderBmp.pixels == nullptr)
    {
      _placeholderBmp.bitsPerPixel = 32;
      _placeholderBmp.height = 1;
      _placeholderBmp.width = 1;
      _placeholderBmp.pixels = (uchar*)&_placeholderBmpData;
    }

    return &_placeholderBmp;
  }

  HBitmap asset_loadBitmap(const char* file)
  {
    LogInfo("Loading Bitmap:\t'%s'", file);

    size_t bufferSize = 0;
    const size_t bitmapStructSize = sizeof(ldk::Bitmap);
    const int8* buffer = (int8*) ldk::platform::loadFileToBufferOffset(
        file, 
        &bufferSize
        ,bitmapStructSize + 1
        ,bitmapStructSize);

    if (!buffer || bufferSize == 0) 
    { 
      return typedHandle_invalid<HBitmap>();
    }

    ldkEngine::memory_tag((void*)buffer, ldkEngine::Allocation::Tag::BITMAP);

    // bitmap data starts after the ldk::Bitmap struct data
    LDK_BITMAP_FILE_HEADER *bitmapHeader =
      (LDK_BITMAP_FILE_HEADER*)(buffer + bitmapStructSize);

    //NOTE: compression info at https://msdn.microsoft.com/en-us/library/cc250415.aspx
    if (bitmapHeader->FileType != BITMAP_FILE_HEADER_SIGNATURE
        || (bitmapHeader->Compression != 0 && bitmapHeader->Compression != 3)
        || bitmapHeader->BitsPerPixel != 16 
        && bitmapHeader->BitsPerPixel != 24
        && bitmapHeader->BitsPerPixel != 32)
    {

      ldk::platform::memoryFree((void*)buffer);
      LogError("Unsupported bitmap type.");

      return typedHandle_invalid<HBitmap>();
    }

    ldk::Bitmap* bitmap = (ldk::Bitmap*)buffer;
    bitmap->bitsPerPixel = bitmapHeader->BitsPerPixel;
    bitmap->pixels = (uint8*)(bitmapHeader) + bitmapHeader->BitmapOffset;
    bitmap->width = bitmapHeader->Width;
    bitmap->height = bitmapHeader->Height;

    //NOTE: Pixel format in memory is ABGR. Must rearrange it as RGBA to make OpenGL happy 
    uint32 numPixels = bitmap->width * bitmap->height;

    if (bitmapHeader->BitsPerPixel == 32)
    {
      for (uint32 i = 0; i < numPixels; i++)
      {
        uint32 argb = *(((uint32*)bitmap->pixels) + i);
        uint32 r = (argb & 0xFF000000) >> 24;
        uint32 g = (argb & 0x00FF0000) >> 8;
        uint32 b = (argb & 0x0000FF00) << 8;
        uint32 a = (argb & 0x000000FF) << 24;
        *((uint32*)bitmap->pixels + i) = r | g | b | a;
      }
    }
    if (bitmapHeader->BitsPerPixel == 24)
    {
      //BGR
      //read 0X99AABBCC
      //     0x00CCBBAA
      for (uint32 i = 0; i < numPixels; i++)
      {
        uint32* pixelPtr = (uint32*)((uint8*)bitmap->pixels + i * 3);

        uint32 argb = *((uint32*)pixelPtr);
        uint32 a = (argb & 0xFF000000); // this is part of the next pixel!
        uint32 r = (argb & 0x00FF0000) >>  16; 
        uint32 g = (argb & 0x0000FF00);
        uint32 b = (argb & 0x000000FF) << 16;
        uint32 color = r | g | b | a;
        *pixelPtr = color;
      }
    }
    else 
    {
      for (uint32 i = 0; i < numPixels; i++)
      {
        uint16 rgb = *(((uint16*)bitmap->pixels) + i);
        *((uint16*)bitmap->pixels + i) = rgb;
      }
    }

    // get a handle for this data
    Handle handle = ldkEngine::handle_store(ldkEngine::HandleType::BITMAP, bitmap);
    return typedHandle_make<HBitmap>(handle);
  }

  ldk::HFont asset_loadFont(const char* file)
  {
    size_t fileSize=0;
    size_t additionalSize = sizeof(ldk::Font);
    char* mem = (char*) ldk::platform::loadFileToBufferOffset(file,
        &fileSize,
        additionalSize,
        additionalSize);

    if (!mem || fileSize < sizeof(ldk::FontData)) return typedHandle_invalid<HFont>();

    ldk::Font* font = (ldk::Font*) mem;
    ldk::FontData* fontData = (ldk::FontData*) (font + 1);
    ldk::Rect* gliphs = (ldk::Rect*) (fontData + 1);

    font->fontData = fontData;
    font->gliphs = gliphs;

    ldkEngine::memory_tag((void*)font, ldkEngine::Allocation::Tag::FONT);
    ldk::Handle fontHandle = ldkEngine::handle_store(ldkEngine::HandleType::FONT, font);
    return typedHandle_make<HFont>(fontHandle);
  }

  static inline void asset_unloadHandle(Handle handle)
  {
    void* dataPtr = ldkEngine::handle_getData(handle);
    ldk::platform::memoryFree(dataPtr);
    ldkEngine::handle_remove(handle);
  }

	void asset_unload(ldk::HBitmap bitmap)
  {
    asset_unloadHandle(bitmap.handle);
  }

	void asset_unload(ldk::HAudio audio)
  {
    asset_unloadHandle(audio.handle);
  }

	void asset_unload(ldk::HFont font)
  {
    asset_unloadHandle(font.handle);
  }

	void asset_unload(ldk::HMesh mesh)
  {
    asset_unloadHandle(mesh.handle);
  }

	void asset_unload(ldk::HMaterial material)
  {
    asset_unloadHandle(material.handle);
  }


  //---------------------------------------------------------------------------
  // Mesh functions
  //---------------------------------------------------------------------------
  ldk::HMesh asset_loadMesh(const char* file)
  {
    // Loads the MeshData from file but reserves space at the beggining for a Mesh
    size_t buffSize;
    ldk::Mesh* mesh = (ldk::Mesh*) ldk::platform::loadFileToBufferOffset(
        file,
        nullptr,
        sizeof(ldk::Mesh),
        sizeof(ldk::Mesh));

    if(!mesh)
      return typedHandle_invalid<HMesh>();

    ldkEngine::memory_tag((void*)mesh, ldkEngine::Allocation::Tag::MESH);

    // Memory layout
    // -----------------------
    // |   MESH   | MESHDATA |
    // -----------------------
    ldk::MeshData* meshData = (ldk::MeshData*) (sizeof(ldk::Mesh) + (char*) mesh);
    mesh->meshData = meshData;
    mesh->indices = (uint32*) (meshData->indicesOffset + (char*) meshData);
    mesh->vertices = (int8*) (meshData->verticesOffset + (char*) meshData);

    ldk::Handle meshHandle = ldkEngine::handle_store(ldkEngine::HandleType::MESH, (void*)mesh);
    return typedHandle_make<HMesh>(meshHandle);
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
      LDK_RIFFAudioChunk *chunk = (LDK_RIFFAudioChunk*) data;
      if ( chunk->signature == fourcc)
      {
        *outSize = chunk->chunkSize;
        return data + sizeof(LDK_RIFFAudioChunk);
      }
      data += chunk->chunkSize + sizeof(LDK_RIFFAudioChunk);
    }
    return nullptr;
  }

  ldk::HAudio asset_loadAudio(const char* file)
  {
    LogInfo("Loading Audio:\t\t'%s'", file);
    size_t bufferSize;
    size_t audioStructSize = sizeof(ldk::Audio);
    int8* buffer = (int8*)ldk::platform::loadFileToBufferOffset(file, &bufferSize, audioStructSize, audioStructSize);

    if (!buffer || bufferSize == 0) return typedHandle_invalid<HAudio>(); 

    ldkEngine::memory_tag((void*)buffer, ldkEngine::Allocation::Tag::AUDIO);
    ldk::Audio* audio = (ldk::Audio*)buffer;
    LDK_RIFFAudioHeaderChunk* riffHeader = (LDK_RIFFAudioHeaderChunk*) (buffer + audioStructSize);
    void* riffData = ((uint8*) buffer + audioStructSize + sizeof(LDK_RIFFAudioHeaderChunk));

    if (riffHeader->signature != LDK_RIFF_FOURCC_RIFF || riffHeader->chunkType != LDK_RIFF_FORMAT_WAVE)
    {
      LogError("Invalid wave file");
      ldk::platform::memoryFree((void*)buffer);
    }

    const ldk::HAudio invalidHandle = typedHandle_invalid<HAudio>();

    // find 'fmt' chunk
    uint32 fmtSize;
    void* fmt = findAudioChunk(riffData, riffHeader->chunkSize, LDK_RIFF_FOURCC_FMT, &fmtSize);
    if (fmt == nullptr) 
    {
      LogError("Error loading wave format table");
      ldk::platform::memoryFree((void*)buffer);
      return invalidHandle;
    }

    // find 'data' chunk
    uint32 dataSize;
    void* data = findAudioChunk(riffData, riffHeader->chunkSize, LDK_RIFF_FOURCC_DATA, &dataSize);
    if ( data == nullptr) 
    {
      LogError("Error loading wave data table");
      ldk::platform::memoryFree((void*)buffer);
      return invalidHandle;
    }

    audio->id = ldk::platform::createAudioBuffer(fmt, fmtSize, data, dataSize);
    ldk::Handle audioHandle = ldkEngine::handle_store(ldkEngine::HandleType::AUDIO, (void*) audio);
    return typedHandle_make<HAudio>(audioHandle);
  }

  LDK_API ldk::HMaterial loadMaterial(const char* file)
  {
    LogInfo("Loading Material:\t'%s'", file);
    return ldk::renderer::loadMaterial(file);
  }

} // namespace ldk

