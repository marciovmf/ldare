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

	LDK_API ldk::Handle mesh_loadFromFile(const char* file);
  LDK_API ldk::Mesh* mesh_getMesh_DEBUG(Handle meshHandle);
  LDK_API bool mesh_getInfo(Handle meshHandle, ldk::MeshInfo* meshInfo);
  LDK_API size_t mesh_copyVertices(Handle meshHandle, void* buffer, uint32 vertexCount);
  LDK_API size_t mesh_copyIndices(Handle meshHandle, void* buffer, uint32 indexCount);
}

#endif // _LDK_MESH_H_
