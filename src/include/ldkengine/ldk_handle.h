#ifndef _LDK_HANDLE_H_
#define _LDK_HANDLE_H_

namespace ldkEngine
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

  ldk::Handle handle_store(HandleType assetType, void* data);

  void handle_remove(ldk::Handle handle);

  void* handle_getData(ldk::Handle handle);

  constexpr ldk::Handle handle_invalid();
}
#endif// _LDK_HANDLE_H_
