
namespace ldk
{
  void playAudio(ldk::Handle audioHandle)
  {
    ldk::Audio* audio = (ldk::Audio*) ldkEngine::handle_getData(audioHandle);
    ldk::platform::playAudioBuffer(audio->id);
  }
}
