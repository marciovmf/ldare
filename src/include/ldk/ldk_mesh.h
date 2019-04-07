#ifndef _LDK_MESH_H_
#define _LDK_MESH_H_

namespace ldk
{
  struct VertexPNUV
  {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
  };

  struct MeshData 
  {
    enum VertexFormat
    {
      PNUV
    };

    VertexFormat format;
    uint32 indexCount;
    uint32 vertexCount;
    uint32 totalSize;
    int32 verticesOffset; // offset from MeshData start
    int32 indicesOffset;  // offset from MeshData start
  };

  struct Mesh
  {
    MeshData* meshData;
    uint32* indices;
    int8* vertices;
  };
}

#endif // _LDK_MESH_H_
