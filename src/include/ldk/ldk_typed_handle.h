#ifndef _LDK_TYPED_HANDLE_
#define _LDK_TYPED_HANDLE_

namespace ldkEngine
{
  extern constexpr ldk::Handle handle_invalid();
}

namespace ldk
{
  // This template provides strong type validation for ldk::Handle at compile
  // time so APIs can enforce specific handle types as paremeters.
  // Too bad typedefs aren't strongly typed. Stupid c++ standard.

  template <typename T>
    struct TypedHandle
    {
      ldk::Handle handle;
    };

  struct ResourceBitmap { };
  struct ResourceAudio { };
  struct ResourceFont { };
  struct ResourceMesh { };
  struct ResourceMaterial { };
  struct ResourceRenderable { };
  struct ResourceShader { };

  typedef TypedHandle<ResourceBitmap> HBitmap;
  typedef TypedHandle<ResourceAudio> HAudio;
  typedef TypedHandle<ResourceFont> HFont;
  typedef TypedHandle<ResourceMesh> HMesh;
  typedef TypedHandle<ResourceMaterial> HMaterial;
  typedef TypedHandle<ResourceRenderable> HRenderable;

  // Creates a typed handle from an ldk::Handle
  template <typename T>
    T typedHandle_make(Handle handle)
    {
      T assetHandle;
      assetHandle.handle = handle;
      return assetHandle;
    }

  // Creates an invalid typed handle
  template <typename T>
    T typedHandle_invalid()
    {
      T assetHandle;
      assetHandle.handle = ldkEngine::handle_invalid();
      return assetHandle;
    }

  template <typename T>
    void* typedHandle_getData(T assetHandle)
    {
      return ldkEngine::handle_getData(assetHandle.data);
    }
}
#endif// _LDK_TYPED_HANDLE_
