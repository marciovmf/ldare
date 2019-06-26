
#ifndef _LDK_ASSET_HANDLE_H_
#define _LDK_ASSET_HANDLE_H_

namespace ldk
{
  enum HandleType : uint8
  {
    EMPTY = 0,
    AUDIO,
    BITMAP,
    FONT,
    MESH,
    MATERIAL,
    RENDERABLE,
    SHADER,
  };

  Handle handle_store(HandleType assetType, void* data);

  void handle_remove(Handle handle);

  void* handle_getData(Handle handle);

  constexpr Handle handle_invalid();
}
#endif// _LDK_ASSET_HANDLE_H_
