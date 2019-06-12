
#include "ldk_asset_handle.h"

namespace ldk
{
  struct AssetHandle
  {
    uint16 slot;
    uint8 version;
    HandleType assetType;
  };

  struct HandleEntry
  {
    void* data;
    uint8 version;
    HandleType assetType;
  };

  static struct HandleTable
  {
    static const uint32 LDK_MAX_HANDLES = 65536;
    uint32 usedSlots = 0;
    HandleEntry slot[LDK_MAX_HANDLES];
  } _handleTable = {};

  static inline Handle handle_encode(uint32 slot, uint32 version, HandleType assetType)
  {
    LDK_ASSERT(sizeof(Handle) == sizeof(uint32), "Handle is expected to have sizeof(uint32)");
    uint32 handle = ((uint16) slot) << 16 | ((uint8) version) << 8 | (uint8) assetType;
    return handle;
  }

  static inline AssetHandle handle_decode(Handle handle)
  {
    LDK_ASSERT(sizeof(Handle) == sizeof(uint32), "Handle is expected to have sizeof(uint32)");
    AssetHandle assetHandle;
    assetHandle.slot = (uint16)(handle >> 16);
    assetHandle.version = (uint8) (handle >> 8);
    assetHandle.assetType = (HandleType) ((uint16) handle);
    return assetHandle;
  }

  // This function returns the GLOBAL handle table
  static inline HandleTable& handle_getGlobalHandleTable()
  {
    return _handleTable;
  }

  Handle handle_store(HandleType assetType, void* data)
  {
    HandleTable& handleTable = handle_getGlobalHandleTable();
    int32 slotIndex = HandleTable::LDK_MAX_HANDLES;
    // Use a new empty slot
    if(handleTable.usedSlots < HandleTable::LDK_MAX_HANDLES)
    {
      slotIndex = handleTable.usedSlots;
      ++handleTable.usedSlots;
    }
    else
    {
      // Reuse a freed slot
      for(int32 i = 0; i < HandleTable::LDK_MAX_HANDLES; i++)
      {
        HandleEntry& entry = handleTable.slot[i];
        if(entry.assetType == HandleType::EMPTY)
        {
          slotIndex = i;
          entry.version++; // every time we reuse a handle slot, we increment its version.
          break;
        }
      }
    }

    if(slotIndex == HandleTable::LDK_MAX_HANDLES)
    {
      Handle handle = handle_encode(0, 0, HandleType::EMPTY);
      return handle;
    }

    // update the entry table
    HandleEntry& handleEntry = handleTable.slot[slotIndex];
    handleEntry.assetType = assetType;
    handleEntry.data = data;

    // encode the entry as a handle
    Handle handle = handle_encode(slotIndex, handleEntry.version, handleEntry.assetType);
    return handle;
  }

  void handle_remove(Handle handle)
  {
    HandleTable& handleTable = handle_getGlobalHandleTable();
    AssetHandle assetHandle = handle_decode(handle);
    HandleEntry& entry = handleTable.slot[assetHandle.slot];


    LDK_ASSERT(entry.assetType != HandleType::EMPTY, "Trying to remove an empty handle");

    entry.assetType = HandleType::EMPTY;
    entry.data = nullptr;
  }

  void* handle_getData(Handle handle)
  {
    HandleTable& handleTable = handle_getGlobalHandleTable();
    AssetHandle assetHandle = handle_decode(handle);
    HandleEntry& entry = handleTable.slot[assetHandle.slot];

    LDK_ASSERT(entry.version == assetHandle.version, "Handle Version mismatch");
    LDK_ASSERT(entry.assetType == assetHandle.assetType, "Handle type mismatch");

    return entry.data;
  }
}
