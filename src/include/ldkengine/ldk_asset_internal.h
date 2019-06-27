#ifndef _LDK_ASSET_INTERNAL_H_
#define _LDK_ASSET_INTERNAL_H_
namespace ldk
{
  //---------------------------------------------------------------------------
  // Mesh Asset
  //---------------------------------------------------------------------------

  struct VertexPNUV
  {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
  };

  struct MeshInfo
  {
    enum VertexFormat
    {
      PNUV
    };

    VertexFormat format;
    uint32 indexCount;
    uint32 vertexCount;
    uint32 totalSize;
  };

  struct MeshData 
  {
    MeshInfo info;
    int32 verticesOffset; // offset from MeshData start
    int32 indicesOffset;  // offset from MeshData start
  };

  struct Mesh
  {
    MeshData* meshData;
    uint32* indices;
    int8* vertices;
  };


  //---------------------------------------------------------------------------
  // Bitmap Asset
  //---------------------------------------------------------------------------
	struct Bitmap
	{
		uint32 width;
		uint32 height;
		uint32 bitsPerPixel;
		const uchar *pixels;
	};

  //---------------------------------------------------------------------------
  // Font Asset
  //---------------------------------------------------------------------------
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

  //---------------------------------------------------------------------------
  // Audio Asset
  //---------------------------------------------------------------------------
	struct Audio
	{
		uint32 id;
	};
}
#endif //_LDK_ASSET_INTERNAL_H_
