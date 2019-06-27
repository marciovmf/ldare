#ifndef _LDK_ASSET_H_
#define _LDK_ASSET_H_

namespace ldk
{
/// @defgroup Asset Asset
/// @details Functions for loading and unloading asset files
/// @{

  ///@brief Loads a Bitmap asset from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
	LDK_API ldk::Handle loadBitmap(const char* file);
	
  ///@brief Loads an Audio asset from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
  LDK_API ldk::Handle loadAudio(const char* file);

  ///@brief Loads a Font asset from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
	LDK_API ldk::Handle loadFont(const char* file);

  ///@brief Loads a Mesh asset from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
	LDK_API ldk::Handle loadMesh(const char* file);

  ///@brief Loads a Material from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
  LDK_API ldk::Handle loadMaterial(const char* file);

  ///@brief Loads a Bitmap from file and returns a Handle to it. 
  ///@param file - A file to load the asset from.
  ///@returns the Handle to the asset.
  ///@see Handle
	LDK_API void unloadAsset(ldk::Handle handle);
  ///@}

} // namespace ldk

#endif // _LDK_ASSET_H_
