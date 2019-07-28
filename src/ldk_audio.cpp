namespace ldk
{
  void playAudio(ldk::HAudio audioHandle)
  {
    ldk::Audio* audio = (ldk::Audio*) ldkEngine::handle_getData(audioHandle.handle);
    ldk::platform::playAudioBuffer(audio->id);
  }
}
