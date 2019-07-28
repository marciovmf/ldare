#ifndef _LDK_ASSET_H_
#define _LDK_ASSET_H_

namespace ldkEngine
{
  extern constexpr ldk::Handle handle_invalid();
}

namespace ldk
{
/// @defgroup Asset Asset
/// @details Functions for loading and unloading asset files
/// @{

  ///@brief Loads a Bitmap asset from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
	LDK_API ldk::HBitmap asset_loadBitmap(const char* file);
	
  ///@brief Loads an Audio asset from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
  LDK_API ldk::HAudio asset_loadAudio(const char* file);

  ///@brief Loads a Font asset from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
	LDK_API ldk::HFont asset_loadFont(const char* file);

  ///@brief Loads a Mesh asset from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
	LDK_API ldk::HMesh asset_loadMesh(const char* file);

  ///@brief Loads a Material from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
  LDK_API ldk::HMaterial loadMaterial(const char* file);

  ///@brief Unloads an asset handle
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
	LDK_API void asset_unload(ldk::HBitmap handle);
	LDK_API void asset_unload(ldk::HAudio handle);
	LDK_API void asset_unload(ldk::HFont handle);
	LDK_API void asset_unload(ldk::HMesh handle);
	LDK_API void asset_unload(ldk::HMaterial handle);

  ///@}

} // namespace ldk

#endif // _LDK_ASSET_H_
