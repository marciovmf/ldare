//---------------------------------------------------------------------------
// Minimal XAudio2 interface for dynamicaly loading/initializing XAudio2 prior
// to 2.8. Most of this content was extracted from XAudio2.h from DirectX SDK (June 2010).
//--------------------------------------------------------------------------- 

#include <unknwn.h>
#include <mmreg.h>

#ifdef __cplusplus
#define DECLSPEC_UUID_WRAPPER(x) __declspec(uuid(#x))
#ifdef INITGUID

#define DEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
class DECLSPEC_UUID_WRAPPER(l## - ##w1## - ##w2## - ##b1##b2## - ##b3##b4##b5##b6##b7##b8) className; \
	EXTERN_C const GUID DECLSPEC_SELECTANY CLSID_##className = __uuidof(className)

#define DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
interface DECLSPEC_UUID_WRAPPER(l## - ##w1## - ##w2## - ##b1##b2## - ##b3##b4##b5##b6##b7##b8) interfaceName; \
	EXTERN_C const GUID DECLSPEC_SELECTANY IID_##interfaceName = __uuidof(interfaceName)

#else // INITGUID

#define DEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
class DECLSPEC_UUID_WRAPPER(l## - ##w1## - ##w2## - ##b1##b2## - ##b3##b4##b5##b6##b7##b8) className; \
	EXTERN_C const GUID CLSID_##className

#define DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
interface DECLSPEC_UUID_WRAPPER(l## - ##w1## - ##w2## - ##b1##b2## - ##b3##b4##b5##b6##b7##b8) interfaceName; \
	EXTERN_C const GUID IID_##interfaceName

#endif // INITGUID

#else // __cplusplus

#define DEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	DEFINE_GUID(CLSID_##className, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)

#define DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	DEFINE_GUID(IID_##interfaceName, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)

#endif // __cplusplus


DEFINE_CLSID(XAudio2_7, 5a508685, a254, 4fba, 9b, 82, 9a, 24, b0, 03, 06, af);
DEFINE_CLSID(XAudio2_7_Debug, db05ea35, 0329, 4d4b, a5, 3a, 6d, ea, d0, 3d, 38, 52);
DEFINE_IID(IXAudio2_7, 8bcf1f58, 9fe7, 4583, 8a, c6, e2, ad, c4, 65, c8, bb);


#define XAUDIO2_7_COMMIT_NOW         0
#define XAUDIO2_7_DEFAULT_CHANNELS   0
#define XAUDIO2_7_DEFAULT_SAMPLERATE 0
#define XAUDIO2_7_DEFAULT_FREQ_RATIO 4.0f
#define XAUDIO2_7_DEBUG_ENGINE       0x0001
#define XAUDIO2_7_LOOP_INFINITE      255
#define XAUDIO2_7_VOICE_NOSRC        0x0004

enum XAUDIO2_DEVICE_ROLE
{
	NotDefaultDevice = 0x0,
	DefaultConsoleDevice = 0x1,
	DefaultMultimediaDevice = 0x2,
	DefaultCommunicationsDevice = 0x4,
	DefaultGameDevice = 0x8,
	GlobalDefaultDevice = 0xf,
	InvalidDeviceRole = ~GlobalDefaultDevice
};

struct XAUDIO2_DEVICE_DETAILS
{
	WCHAR DeviceID[256];
	WCHAR DisplayName[256];
	XAUDIO2_DEVICE_ROLE Role;
	WAVEFORMATEXTENSIBLE OutputFormat;
};

//TODO:(marcio) enable this IF necessary or delete if not going to be used
#if 0
	 DECLARE_INTERFACE(IXAudio2Voice);
struct XAUDIO2_7_DEVICE_DETAILS
{
	WCHAR DeviceID[256];
	WCHAR DisplayName[256];
	XAUDIO2_7_DEVICE_ROLE Role;
	WAVEFORMATEXTENSIBLE OutputFormat;
};

struct XAUDIO2_7_VOICE_DETAILS
{
	UINT32 CreationFlags;
	UINT32 InputChannels;
	UINT32 InputSampleRate;
};

typedef enum XAUDIO2_7_WINDOWS_PROCESSOR_SPECIFIER
{
	Processor1 = 0x00000001,
	Processor2 = 0x00000002,
	Processor3 = 0x00000004,
	Processor4 = 0x00000008,
	Processor5 = 0x00000010,
	Processor6 = 0x00000020,
	Processor7 = 0x00000040,
	Processor8 = 0x00000080,
	Processor9 = 0x00000100,
	Processor10 = 0x00000200,
	Processor11 = 0x00000400,
	Processor12 = 0x00000800,
	Processor13 = 0x00001000,
	Processor14 = 0x00002000,
	Processor15 = 0x00004000,
	Processor16 = 0x00008000,
	Processor17 = 0x00010000,
	Processor18 = 0x00020000,
	Processor19 = 0x00040000,
	Processor20 = 0x00080000,
	Processor21 = 0x00100000,
	Processor22 = 0x00200000,
	Processor23 = 0x00400000,
	Processor24 = 0x00800000,
	Processor25 = 0x01000000,
	Processor26 = 0x02000000,
	Processor27 = 0x04000000,
	Processor28 = 0x08000000,
	Processor29 = 0x10000000,
	Processor30 = 0x20000000,
	Processor31 = 0x40000000,
	Processor32 = 0x80000000,
	XAUDIO2_7_ANY_PROCESSOR = 0xffffffff,
	XAUDIO2_7_DEFAULT_PROCESSOR = XAUDIO2_7_ANY_PROCESSOR
} XAUDIO2_7_WINDOWS_PROCESSOR_SPECIFIER, XAUDIO2_7_PROCESSOR;

struct XAUDIO2_7_VOICE_SENDS
{
	UINT32 OutputCount;
	IXAudio2Voice* *pOutputVoices;
};

struct XAUDIO2_7_EFFECT_DESCRIPTOR
{
	IUnknown *pEffect;
	BOOL InitialState;
	UINT32 OutputChannels;
};

struct XAUDIO2_7_EFFECT_CHAIN
{
	UINT32 EffectCount;
	const XAUDIO2_7_EFFECT_DESCRIPTOR *pEffectDescriptors;
};

enum XAUDIO2_7_FILTER_TYPE
{
	LowPassFilter,
	BandPassFilter,
	HighPassFilter
};

struct XAUDIO2_7_FILTER_PARAMETERS
{
	XAUDIO2_7_FILTER_TYPE Type;
	float Frequency;
	float OneOverQ;
};

struct XAUDIO2_7_BUFFER
{
	UINT32 Flags;
	UINT32 AudioBytes;
	const BYTE *pAudioData;
	UINT32 PlayBegin;
	UINT32 PlayLength;
	UINT32 LoopBegin;
	UINT32 LoopLength;
	UINT32 LoopCount;
	void *pContext;
};

struct XAUDIO2_7_BUFFER_WMA
{
	const UINT32 *pDecodedPacketCumulativeBytes;
	UINT32 PacketCount;
};

struct XAUDIO2_7_VOICE_STATE
{
	void *pCurrentBufferContext;
	UINT32 BuffersQueued;
	UINT64 SamplesPlayed;
};

struct XAUDIO2_7_PERFORMANCE_DATA
{
	UINT64 AudioCyclesSinceLastQuery;
	UINT64 TotalCyclesSinceLastQuery;
	UINT32 MinimumCyclesPerQuantum;
	UINT32 MaximumCyclesPerQuantum;
	UINT32 MemoryUsageInBytes;
	UINT32 CurrentLatencyInSamples;
	UINT32 GlitchesSinceEngineStarted;
	UINT32 ActiveSourceVoiceCount;
	UINT32 TotalSourceVoiceCount;
	UINT32 ActiveSubmixVoiceCount;
	UINT32 TotalSubmixVoiceCount;
	UINT32 ActiveXmaSourceVoices;
	UINT32 ActiveXmaStreams;
};

struct XAUDIO2_7_DEBUG_CONFIGURATION
{
	UINT32 TraceMask;
	UINT32 BreakMask;
	BOOL LogThreadID;
	BOOL LogFileline;
	BOOL LogFunctionName;
	BOOL LogTiming;
};

DECLARE_INTERFACE(IXAudio2EngineCallback)
{
	STDMETHOD_(void, OnProcessingPassStart) (void);
	STDMETHOD_(void, OnProcessingPassEnd) (void);
	STDMETHOD_(void, OnCriticalError) (HRESULT Error);
};

DECLARE_INTERFACE(IXAudio2VoiceCallback)
{
	STDMETHOD_(void, OnVoiceProcessingPassStart) (UINT32 BytesRequired);
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (void);
	STDMETHOD_(void, OnStreamEnd) (void);
	STDMETHOD_(void, OnBufferStart) (void *pBufferContext);
	STDMETHOD_(void, OnBufferEnd) (void *pBufferContext);
	STDMETHOD_(void, OnLoopEnd) (void *pBufferContext);
	STDMETHOD_(void, OnVoiceError) (void *pBufferContext, HRESULT Error);
};

DECLARE_INTERFACE(IXAudio2Voice)
{
	STDMETHOD_(void, GetVoiceDetails) (XAUDIO2_7_VOICE_DETAILS *pVoiceDetails);
	STDMETHOD(SetOutputVoices) (const XAUDIO2_7_VOICE_SENDS *pSendList);
	STDMETHOD(SetEffectChain) (const XAUDIO2_7_EFFECT_CHAIN *pEffectChain);
	STDMETHOD(EnableEffect) (UINT32 EffectIndex, UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD(DisableEffect) (UINT32 EffectIndex, UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD_(void, GetEffectState) (UINT32 EffectIndex, BOOL *pEnabled);
	STDMETHOD(SetEffectParameters) (UINT32 EffectIndex, const void *pParameters, UINT32 ParametersByteSize,
		UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD(GetEffectParameters) (UINT32 EffectIndex, void *pParameters, UINT32 ParametersByteSize);
	STDMETHOD(SetFilterParameters) (const XAUDIO2_7_FILTER_PARAMETERS *pParameters, UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD_(void, GetFilterParameters) (XAUDIO2_7_FILTER_PARAMETERS *pParameters);
	STDMETHOD(SetVolume) (float Volume, UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD_(void, GetVolume) (float *pVolume);
	STDMETHOD(SetChannelVolumes) (UINT32 Channels, const float *pVolumes, UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD_(void, GetChannelVolumes) (UINT32 Channels, float *pVolumes);
	STDMETHOD(SetOutputMatrix) (IXAudio2Voice *pDestinationVoice, UINT32 SourceChannels, UINT32 DestinationChannels,
		const float *pLevelMatrix, UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD_(void, GetOutputMatrix) (IXAudio2Voice *pDestinationVoice, UINT32 SourceChannels,
		UINT32 DestinationChannels, float *pLevelMatrix);
	STDMETHOD_(void, DestroyVoice) (void);
};

DECLARE_INTERFACE_(IXAudio2MasteringVoice, IXAudio2Voice){};

DECLARE_INTERFACE_(IXAudio2SubmixVoice, IXAudio2Voice){};

DECLARE_INTERFACE_(IXAudio2SourceVoice, IXAudio2Voice)
{
	STDMETHOD(Start) (UINT32 Flags, UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD(Stop) (UINT32 Flags, UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD(SubmitSourceBuffer) (const XAUDIO2_7_BUFFER *pBuffer, const XAUDIO2_7_BUFFER_WMA *pBufferWMA = NULL);
	STDMETHOD(FlushSourceBuffers) (void);
	STDMETHOD(Discontinuity) (void);
	STDMETHOD(ExitLoop) (UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD_(void, GetState) (XAUDIO2_7_VOICE_STATE *pVoiceState);
	STDMETHOD(SetFrequencyRatio) (float Ratio, UINT32 OperationSet = XAUDIO2_7_COMMIT_NOW);
	STDMETHOD_(void, GetFrequencyRatio) (float *pRatio);
};
#endif // 0

DECLARE_INTERFACE_(IXAudio2_7, IUnknown)
{
	STDMETHOD(GetDeviceCount) (UINT32 *pCount);
	STDMETHOD(GetDeviceDetails) (UINT32 Index, XAUDIO2_DEVICE_DETAILS *pDeviceDetails);
	STDMETHOD(Initialize) (UINT32 Flags = 0, XAUDIO2_PROCESSOR XAudio2Processor = XAUDIO2_DEFAULT_PROCESSOR);
	STDMETHOD(RegisterForCallbacks) (IXAudio2EngineCallback *pCallback);
	STDMETHOD_(void, UnregisterForCallbacks) (IXAudio2EngineCallback *pCallback);
	STDMETHOD(CreateSourceVoice) (IXAudio2SourceVoice* *ppSourceVoice, const WAVEFORMATEX *pSourceFormat, UINT32 Flags = 0,
		float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO, IXAudio2VoiceCallback *pCallback = NULL,
		const XAUDIO2_VOICE_SENDS *pSendList = NULL, const XAUDIO2_EFFECT_CHAIN *pEffectChain = NULL);
	STDMETHOD(CreateSubmixVoice) (IXAudio2SubmixVoice* *ppSubmixVoice, UINT32 InputChannels, UINT32 InputSampleRate,
		UINT32 Flags = 0, UINT32 ProcessingStage = 0, const XAUDIO2_VOICE_SENDS *pSendList = NULL,
		const XAUDIO2_EFFECT_CHAIN *pEffectChain = NULL);
	STDMETHOD(CreateMasteringVoice) (IXAudio2MasteringVoice* *ppMasteringVoice,
		UINT32 InputChannels = XAUDIO2_DEFAULT_CHANNELS, UINT32 InputSampleRate = XAUDIO2_DEFAULT_SAMPLERATE,
		UINT32 Flags = 0, UINT32 DeviceIndex = 0, const XAUDIO2_EFFECT_CHAIN *pEffectChain = NULL);
	STDMETHOD(StartEngine) (void);
	STDMETHOD_(void, StopEngine) (void);
	STDMETHOD(CommitChanges) (UINT32 OperationSet);
	STDMETHOD_(void, GetPerformanceData) (XAUDIO2_PERFORMANCE_DATA *pPerfData);
	STDMETHOD_(void, SetDebugConfiguration) (const XAUDIO2_DEBUG_CONFIGURATION *pDebugConfiguration, void *pReserved = NULL);
};
IXAudio2_7* InitXAudio2_7()
{
	// Instantiate the appropriate XAudio2 engine
	IXAudio2_7* pXAudio2;


#ifdef __cplusplus

	HRESULT hr = CoCreateInstance(__uuidof(XAudio2_7),
		NULL, CLSCTX_INPROC_SERVER, __uuidof(IXAudio2_7), (void**)&pXAudio2);
	if (SUCCEEDED(hr))
	{
	
		hr = pXAudio2->Initialize(0, XAUDIO2_DEFAULT_PROCESSOR);

		if (FAILED(hr))
		{
			pXAudio2->Release();
		}
	}

#else

	HRESULT hr = CoCreateInstance((Flags & XAUDIO2_7_DEBUG_ENGINE) ? &CLSID_XAudio2_7_Debug : &CLSID_XAudio2_7,
		NULL, CLSCTX_INPROC_SERVER, &IID_IXAudio2, (void**)&pXAudio2_7);
	if (SUCCEEDED(hr))
	{
		hr = pXAudio2->lpVtbl->Initialize(pXAudio2, Flags, XAudio2Processor);

		if (SUCCEEDED(hr))
		{
			LogInfo("XAudio2 (legacy) initialized");
		}
		else
		{
			LogError("Failed to initialize legacy XAudio2");
			pXAudio2->lpVtbl->Release(pXAudio2);
		}
	}

#endif // #ifdef __cplusplus

	return pXAudio2;
}

