#ifndef _LDK_WAV_H_
#define _LDK_WAV_H_

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
  //---------------------------------------------------------------------------
  // WAV file format specifics
  //---------------------------------------------------------------------------
  struct LDK_RIFFAudioHeaderChunk
  {
    uint32 signature;
    uint32 chunkSize;
    uint32 chunkType;
  };

  struct LDK_RIFFAudioChunk
  {
    uint32 signature;
    uint32 chunkSize;
  };

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#define LDK_RIFF_FORMAT_WAVE 0x45564157
#define LDK_RIFF_FOURCC_RIFF 0x46464952
#define LDK_RIFF_FOURCC_FMT 0x20746d66
#define LDK_RIFF_FOURCC_DATA 0x61746164

#endif  // _LDK_WAV_H_
