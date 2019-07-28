#ifndef _LDK_AUDIO_H_
#define _LDK_AUDIO_H_

namespace ldk
{
  /// @defgroup Config Config
  /// @details Functions for interacting and reproducing audio assets
  /// @{

  ///@brief Plays an audio asset
  ///@param audioHandle - A handle to an audio asset
  ///@returns void
  ///@see Handle
  LDK_API void playAudio(ldk::HAudio audioHandle);
  ///@}
}

#endif  // _LDK_AUDIO_H_
