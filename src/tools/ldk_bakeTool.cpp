
#include <stdlib.h>
#include <stdio.h>
#include <ldk/ldk.h>
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

static bool loadArgumentFile(ArgumentFile* argumentFile, const char* path)
{
  FILE* fd = fopen(path, "r");
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
  if(totalRead != fileSize)
  {
    printf("[ERROR]: Could not read file '%s'", path);
    return false;
  }

  fclose(fd);

  argumentFile->name = path;
  argumentFile->size = fileSize;
  argumentFile->mem = mem;
  return true;
}

static void destroyArgumentFile(ArgumentFile* argumentFile)
{
  free(argumentFile->mem);
}

static bool bakeMeshFromObjFile(ArgumentFile* argumentFile, const char* outputFileName)
{
  return ldk::parseObjFile((const char*)argumentFile->mem, argumentFile->size);
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

  bakeMeshFromObjFile(&input, outputFileName);
    
  destroyArgumentFile(&input);
  return LDK_EXIT_SUCCESS;
}
