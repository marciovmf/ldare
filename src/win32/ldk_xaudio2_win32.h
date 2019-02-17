//---------------------------------------------------------------------------
// Minimal XAudio2 interface for dynamicaly loading/initializing XAudio2 prior
// to 2.8. Most of this content was extracted from XAudio2.h from DirectX SDK (June 2010).
//--------------------------------------------------------------------------- 
#ifndef _LDK_XAUDIO2_WIN32_H_
#define _LDK_XAUDIO2_WIN32_H_

#include <XAudio2.h> 							// Default Windows SDK XAudio header
#include <unknwn.h>
#include <mmreg.h>

// All structures defined in this file use tight field packing
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif 

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

#define XAUDIO2_DEBUG_ENGINE       0x0001

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

IXAudio2_7* XAudio2_7Create(uint32 flags X2DEFAULT(0))
{
	// Instantiate the appropriate XAudio2 engine
	IXAudio2_7* pXAudio2_7;

#ifdef __cplusplus

	HRESULT hr = CoCreateInstance((flags & XAUDIO2_DEBUG_ENGINE) ? __uuidof(XAudio2_7_Debug) :
			__uuidof(XAudio2_7), NULL, CLSCTX_INPROC_SERVER, __uuidof(IXAudio2_7), (void**)&pXAudio2_7);
	if (SUCCEEDED(hr))
	{
		hr = pXAudio2_7->Initialize(0, XAUDIO2_DEFAULT_PROCESSOR);

		if (FAILED(hr))
		{
			pXAudio2_7->Release();
		}
	}

#else 

	HRESULT hr = CoCreateInstance((flags & XAUDIO2_DEBUG_ENGINE) ? &CLSID_XAudio2_Debug : &CLSID_XAudio2_7,
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
			pXAudio2->lpVtbl->Release(pXAudio2_7);
		}
	}

#endif // #ifdef __cplusplus
	return  pXAudio2_7;
}
#ifdef _MSC_VER
#pragma pack(pop)
#endif

namespace ldk
{
	namespace platform
	{
		typedef decltype(&XAudio2Create) XAudio2CreateFunc;
		static IXAudio2* pXAudio2 = nullptr;
		static IXAudio2_7* pXAudio2_7 = nullptr; 

		// represents an audio buffer bound to a source voice
		struct BoundAudio
		{
			XAUDIO2_BUFFER buffer;
			IXAudio2SourceVoice* voice;
		};

		/* Initializes XAudio2 */
		static void ldk_win32_initXAudio()
		{
			XAudio2CreateFunc ptrXAudio2Create = nullptr;
			const char* ErrorInitializingMsg = "Error initializing %s.";
			bool usingLegacyXAudio = false;

			// XAudio2.9
			char* xAudioDllName = "xaudio2_9.dll"; 
			HMODULE xAudioLib = LoadLibraryA(xAudioDllName);
			if (!xAudioLib)
			{
				xAudioDllName = "xaudio2_8.dll";
				xAudioLib = LoadLibraryA(xAudioDllName);
			}
			// XAudio2.8
			if (!xAudioLib)
			{
				xAudioDllName = "xaudio2_7.dll";
				xAudioLib = LoadLibraryA(xAudioDllName);
				usingLegacyXAudio = true;
			}
			// XAudio2.7 (Old creepy COM dll)
			if (!xAudioLib)
			{
				LogError("could not initialize XAudio2. No suitable xinput dll found.");
				return;
			}

			HRESULT hr;
			IXAudio2MasteringVoice* pMasterVoice = NULL;

			if (usingLegacyXAudio)
			{
				//pXAudio2 = XAudio2_7Create(XAUDIO2_DEBUG_ENGINE);
				pXAudio2_7 = XAudio2_7Create();

				if (pXAudio2_7 <= 0)
				{
					LogError(ErrorInitializingMsg, xAudioDllName);
					return;
				}
				pXAudio2_7->StartEngine();
#ifdef _LDK_DEBUG_
				XAUDIO2_DEBUG_CONFIGURATION debug = {};
				debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
				debug.BreakMask = XAUDIO2_LOG_ERRORS;
				pXAudio2_7->SetDebugConfiguration( &debug, 0 );

				pXAudio2 = (IXAudio2*) pXAudio2_7;
#endif //_LDK_DEBUG_
				hr = pXAudio2_7->CreateMasteringVoice( &pMasterVoice);
			}
			else
			{
				ptrXAudio2Create = (XAudio2CreateFunc) GetProcAddress(xAudioLib, "XAudio2Create");
				hr = ptrXAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
				if (FAILED(hr))
				{
					LogError(ErrorInitializingMsg, xAudioDllName);
					return;
				}
				hr = pXAudio2->CreateMasteringVoice( &pMasterVoice);
			}


			if (FAILED(hr))
			{
				LogError("Could not init XAudio2", xAudioDllName);
				return;
			}
			LogInfo("XAudio2 %s initialized.", xAudioDllName);
		}
	}
}
#endif // _LDK_XAUDIO2_WIN32_H_
