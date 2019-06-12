
#ifndef _LDK_ASSET_HANDLE_H_
#define _LDK_ASSET_HANDLE_H_

namespace ldk
{
  typedef uint32 Handle;
  struct HandleTable;
  
  enum HandleType : uint8
  {
    EMPTY,
    RAW,
    BITMAP,
    AUDIO,
    MESH,
    SHADER,
    MATERIAL
  };

  Handle handle_store(HandleTable& handleTable, HandleType assetType, void* data);

  void handle_remove(HandleTable& handleTable, Handle handle);

  void* handle_getData(HandleTable& handleTable, Handle handle);
}
#endif// _LDK_ASSET_HANDLE_H_
