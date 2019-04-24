#include <stdlib.h>
#include <stdio.h>
#include <ldk/ldk.h>

ldk::MeshData* createMeshDataPNUV(
    ldk::VertexPNUV* vertexData,
    uint32 vertexCount,
    uint32* indexData,
    uint32 indexCount);

#include "ldk_objImporter.cpp"

static int32 showUsage()
{
  printf("Usage:\n bake inputFile outputFile\n");
  return LDK_EXIT_SUCCESS;
}

struct ArgumentFile
{
  const char* name;
  size_t size;
  int8* mem;
};


ldk::MeshData* createMeshDataPNUV(
    ldk::VertexPNUV* vertexData,
    uint32 vertexCount,
    uint32* indexData,
    uint32 indexCount)
{
  size_t indexDataSize = indexCount * sizeof(uint32);
  size_t vertexDataSize = vertexCount * sizeof(ldk::VertexPNUV);
  size_t meshDataSize = sizeof(ldk::MeshData);

  size_t totalSize = meshDataSize
    + indexDataSize
    + vertexDataSize;

  int8* mem = (int8*) malloc(totalSize);

  // MeshData | indices | vertices
  int8* indicesPtr =  mem + meshDataSize;
  int8* verticesPtr = mem + meshDataSize + indexDataSize;

  ldk::MeshData* mesh = (ldk::MeshData*) mem;
  mesh->format = ldk::MeshData::PNUV;
  mesh->indexCount = indexCount;
  mesh->vertexCount = vertexCount;
  mesh->indicesOffset = indicesPtr - mem;
  mesh->verticesOffset = verticesPtr - mem;
  mesh->totalSize = totalSize;

  // write indices
  memcpy(indicesPtr, indexData, indexDataSize);
  memcpy(verticesPtr, vertexData, vertexDataSize);

  return mesh;
}

void destroyMesh(ldk::Mesh* mesh)
{
  free(mesh);
}

static bool loadArgumentFile(ArgumentFile* argumentFile, const char* path)
{
  bool result;
  FILE* fd = fopen(path, "rb");
  if (fd == 0)
  {
    printf("[ERROR]: Could not open file '%s'\n", path);
    return false;
  }

  fseek(fd, 0, SEEK_END);
  long fileSize = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  // read the whole file
  int8* mem = (int8*) malloc(fileSize * sizeof(int8));
  size_t totalRead = fread(mem, sizeof(int8), fileSize, fd);

  if(totalRead == fileSize)
  {
    argumentFile->name = path;
    argumentFile->size = fileSize;
    argumentFile->mem = mem;
    result = true;
  }
  else
  {
    printf("[ERROR]: Could not read file '%s'", path);
    free(mem);
    result = false;
  }
  
  fclose(fd);
  return result;
}

static void destroyArgumentFile(ArgumentFile* argumentFile)
{
  free(argumentFile->mem);
}

static bool bakeMeshFromObjFile(ArgumentFile* argumentFile, FILE* fd)
{
  const ldk::MeshData* meshData = ldk::parseObjFile((const char*)argumentFile->mem, argumentFile->size);
  if(meshData == nullptr) return false;

  size_t result = fwrite((const void*)meshData, sizeof(int8), meshData->totalSize, fd);
  free((void*)meshData);
  return result == meshData->totalSize;
}

int32 main(int argc, char** argv)
{
  if (argc != 3)
    return showUsage();

  const char* inputFileName = argv[1];
  const char* outputFileName = argv[2];

  ArgumentFile input;

  if(!loadArgumentFile(&input, inputFileName))
  {
    return LDK_EXIT_FAIL;
  }

  FILE* outFile = fopen(outputFileName, "wb");
  if(!outFile) return LDK_EXIT_FAIL;

  bool bakeResult = bakeMeshFromObjFile(&input, outFile);
  if(!bakeResult) return LDK_EXIT_FAIL;

  fclose(outFile);
  destroyArgumentFile(&input);
  return LDK_EXIT_SUCCESS;
}


