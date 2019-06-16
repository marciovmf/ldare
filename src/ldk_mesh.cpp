
namespace ldk
{

  //ldk::MeshData* mesh_loadFromFile(const char* file)
  //{
  //  size_t buffSize;
  //  ldk::MeshData* meshData = (ldk::MeshData*) ldk::platform::loadFileToBuffer(file, &buffSize);
  //  return meshData;
  //}

  ldk::Handle mesh_loadFromFile(const char* file)
  {

    // Loads the MeshData from file but reserves space at the beggining for a Mesh
    size_t buffSize;
    ldk::Mesh* mesh = (ldk::Mesh*) ldk::platform::loadFileToBufferOffset(
          file,
          nullptr,
          sizeof(ldk::Mesh),
          sizeof(ldk::Mesh));

    if(!mesh)
      return handle_invalid();

    // Memory layout
    // ------------
    // |   MESH   | <- setup by us now
    // | MESHDATA | <- loaded from file
    // ------------
    ldk::MeshData* meshData = (ldk::MeshData*) (sizeof(ldk::Mesh) + (char*) mesh);
    mesh->meshData = meshData;
    mesh->indices = (uint32*) (meshData->indicesOffset + (char*) meshData);
    mesh->vertices = (int8*) (meshData->verticesOffset + (char*) meshData);

    ldk::Handle handle = handle_store(HandleType::MESH, (void*)mesh);
    return handle;
  }

  ldk::Mesh* mesh_getMesh_DEBUG(Handle meshHandle)
  {
    return (ldk::Mesh*) handle_getData(meshHandle);
  }

  bool mesh_getInfo(Handle meshHandle, ldk::MeshInfo* meshInfo)
  {
    ldk::Mesh* mesh = (ldk::Mesh*) handle_getData(meshHandle);
    if(!mesh) return false;

    *meshInfo = mesh->meshData->info;
    return true;
  }


  static inline size_t mesh_getVertexSizeInternal(ldk::MeshInfo::VertexFormat format)
  {
    switch (format)
    {
      case ldk::MeshInfo::VertexFormat::PNUV:
        return sizeof(ldk::VertexPNUV);
      default:
        LogError("Unknown Vertex type");
        return 0;
    }
  }

  size_t mesh_copyVertices(Handle meshHandle, void* buffer, uint32 vertexCount)
  {
      ldk::Mesh* mesh = (ldk::Mesh*)ldk::handle_getData(meshHandle);
      if(!mesh) return 0;


      size_t numVertices = vertexCount <= mesh->meshData->info.indexCount
                              ? (size_t) vertexCount
                              : mesh->meshData->info.vertexCount;

      ldk::MeshInfo::VertexFormat format = mesh->meshData->info.format;
      size_t vertexSize = mesh_getVertexSizeInternal(format);
      size_t totalBytes = vertexSize * vertexCount;
      memcpy(buffer, mesh->vertices, totalBytes);
      return totalBytes;
  }

  size_t mesh_copyIndices(Handle meshHandle, void* buffer, uint32 indexCount)
  {
      ldk::Mesh* mesh = (ldk::Mesh*)ldk::handle_getData(meshHandle);
      if(!mesh) return 0;


      size_t numIndices = indexCount <= mesh->meshData->info.indexCount 
                              ? indexCount
                              : mesh->meshData->info.indexCount;

      const size_t indexSize = sizeof(uint32);
      size_t totalBytes = indexSize * indexCount;
      memcpy(buffer, mesh->indices, totalBytes);
      return totalBytes;
  }
}
