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

  const uint32 LDK_MAX_FONT_NAME_LEN = 32;
  const uint32 LDK_FONT_HEADER_SIGNATURE = 0x544e4f46; // 'F''O''N''T'

  struct FontData
  {
    uint32 signature;
    char8 name[LDK_MAX_FONT_NAME_LEN];
    uint16 firstCodePoint;  // first character codepoint defined in the font
    uint16 lastCodePoint;  	// last character codepoint defined in the font
    uint16 defaultCodePoint;// default character to be substituted in the font
    uint16 size;            // size of the font
  };

  struct Font
  {
    FontData* fontData;
    Rect* gliphs;
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
