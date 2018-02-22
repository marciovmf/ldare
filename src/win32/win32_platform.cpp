/**
 * win32_platform.h
 * Win32 implementation for ldare platform functions
 */
#include <XAudio2.h> 							// Default Windows SDK XAudio header
#include "win32_ldare_xaudio2.h" 	// Custom XAudio header form old XAudio <= 2.7

#define XINPUT_GAMEPAD_DPAD_UP	0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN	0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT	0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT	0x0008
#define XINPUT_GAMEPAD_START	0x0010
#define XINPUT_GAMEPAD_BACK	0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB	0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB	0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER	0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER	0x0200
#define XINPUT_GAMEPAD_A	0x1000
#define XINPUT_GAMEPAD_B	0x2000
#define XINPUT_GAMEPAD_X	0x4000
#define XINPUT_GAMEPAD_Y	0x8000

//#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  9000
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30
#define XINPUT_MAX_AXIS_VALUE 32767
#define XINPUT_MIN_AXIS_VALUE -32768
#define XINPUT_MAX_TRIGGER_VALUE 255

namespace ldare 
{
	// Platform implementation
	namespace platform 
	{
		struct GameModule
		{	
			const char* moduleFileName;
			HMODULE hGameModule;
			FILETIME gameModuleWriteTime;
			gameInitFunc *init;
			gameStartFunc *start;
			gameUpdateFunc *update;
			gameStopFunc *stop;
			float timeSinceLastReload;
		};

		//---------------------------------------------------------------------------
		// Get the write time of a file
		//---------------------------------------------------------------------------
		static FILETIME Win32_getFileWriteTime(const char* fileName)
		{
			FILETIME writeTime;
			HANDLE handle = CreateFileA(fileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			GetFileTime(handle, 0, 0, &writeTime);
			CloseHandle(handle);
			return writeTime;
		}

		//---------------------------------------------------------------------------
		// Checks if there is a newer game dll and loads it if it does.
		// Returns: true if the module was loaded or reloaded
		//---------------------------------------------------------------------------
		static bool Win32_reloadGameModule(ldare::platform::GameModule& gameModule)
		{
			bool hasNewVersion = false;
			if (gameModule.hGameModule != 0)
			{
				FILETIME writeTime = Win32_getFileWriteTime(gameModule.moduleFileName);
				// game dll has a recenb write time change ?
				if (CompareFileTime(&writeTime, &gameModule.gameModuleWriteTime) > 0)
				{
					LogInfo("New game module found...");
					ldare::platform::unloadGameModule(gameModule);
					gameModule.gameModuleWriteTime = writeTime;
					hasNewVersion = true;
				}
			}

			if (hasNewVersion)
				return ldare::platform::loadGameModule(gameModule);

			return false;
		}

		//---------------------------------------------------------------------------
		// Retrieves an opengGl function pointer
		// Returns: true if the module was loaded or reloaded
		//---------------------------------------------------------------------------
		static void* Win32_getGlFunctionPointer(const char* functionName)
		{
			static HMODULE opengl32dll = GetModuleHandleA("OpenGL32.dll");
			void* functionPtr = wglGetProcAddress(functionName);
			if( functionPtr == (void*)0x1 || functionPtr == (void*) 0x02 ||
					functionPtr == (void*) 0x3 || functionPtr == (void*) -1 ||
					functionPtr == (void*) 0x0)
			{
				functionPtr = GetProcAddress(opengl32dll, functionName);
				if(!functionPtr)
				{
					LogError("Could not get GL function pointer");
					LogError(functionName);
					return nullptr;
				}
			}

			return functionPtr;
		}

		void Win32_setCurrentDirectory()
		{
			char path[256];
			uint32 len = GetModuleFileName(NULL, path, 255);
			char* p=path+len;
			while( *p!= '\\')
			{
				*p=0;
				p--;
			}
			SetCurrentDirectory(path);
			LogInfo("Running from %s", path);
		}
		//---------------------------------------------------------------------------
		// Input specific structures and
		//---------------------------------------------------------------------------
		typedef struct _XINPUT_GAMEPAD 
		{
			WORD  wButtons;
			BYTE  bLeftTrigger;
			BYTE  bRightTrigger;
			SHORT sThumbLX;
			SHORT sThumbLY;
			SHORT sThumbRX;
			SHORT sThumbRY;
		} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

		typedef struct _XINPUT_STATE 
		{
			DWORD dwPacketNumber;
			XINPUT_GAMEPAD Gamepad;
		} XINPUT_STATE, *PXINPUT_STATE;

#define XINPUT_GET_STATE_FUNC(name) DWORD name(DWORD dwUserIndex, XINPUT_STATE *pState)
		typedef XINPUT_GET_STATE_FUNC(XInputGetStateFunc);
		XInputGetStateFunc* XInputGetState = nullptr;
		XINPUT_GET_STATE_FUNC(XInputGetStateStub)
		{
			return ERROR_DEVICE_NOT_CONNECTED;
		}

		static struct Win32TimerData
		{
			LARGE_INTEGER ticksPerSecond;
			LARGE_INTEGER ticksSinceEngineStartup;
		} _timerData;

		void* memoryAlloc(size_t size)
		{
			ASSERT(size>0, "allocation size must be greater than zero");
			void* mem =
				VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (mem==0) { LogError("Error allocating memory"); }
			return mem;
		}

		void* loadFileToBuffer(const char* fileName, size_t* bufferSize)
		{
			HANDLE hFile = CreateFile((LPCSTR)fileName,
					GENERIC_READ,
					FILE_SHARE_READ,
					0,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					0);

			DWORD err = GetLastError();

			if (hFile == INVALID_HANDLE_VALUE) 
			{
				LogError("Could not open file '%s'", fileName);
				return nullptr;
			}

			int32 fileSize;
			fileSize = GetFileSize(hFile, 0);

			if ( bufferSize != nullptr) { *bufferSize = fileSize; }
			//TODO: alloc memory from the proper Heap
			void *buffer = memoryAlloc(fileSize);
			uint32 bytesRead;
			if (!buffer || ReadFile(hFile, buffer, fileSize, (LPDWORD)&bytesRead, 0) == 0)
			{
				err = GetLastError();
				LogError("%d Could not read file '%s'", err, fileName);
				return nullptr;
			}

			CloseHandle(hFile);
			return buffer;
		}

		void memoryFree(void* memory, size_t size)
		{
			VirtualFree(memory, size, MEM_DECOMMIT);	
		}
		
		//---------------------------------------------------------------------------
		//  
		//  Timer structures and functions
		//
		//---------------------------------------------------------------------------
		void Win32_initTimer()
		{
			QueryPerformanceFrequency(&_timerData.ticksPerSecond);
			QueryPerformanceCounter(&_timerData.ticksSinceEngineStartup);
		}

		uint64 getTicks()
		{
			LARGE_INTEGER value;
			QueryPerformanceCounter(&value);
			return value.QuadPart;
		}

		float getTimeBetweenTicks(uint64 start, uint64 end)
		{ 
			end -= _timerData.ticksSinceEngineStartup.QuadPart;
			start -= _timerData.ticksSinceEngineStartup.QuadPart;

			float deltaTime = (( end - start))/ (float)_timerData.ticksPerSecond.QuadPart;
#if _LDK_DEBUG_
			// if we stopped on a breakpoint, make things behave mor natural
			if ( deltaTime > 0.05f)
				deltaTime = 0.016f;
#endif //_LDK_DEBUG_
			return deltaTime;
		}
		
		bool unloadGameModule(GameModule& gameModule)
		{
			if (!gameModule.hGameModule)
			{
				return true;
			}

			return FreeLibrary(gameModule.hGameModule);
		}

		bool loadGameModule(GameModule& gameModule)
		{
			//TODO: marcio, make sure executable directory is current directory.
			const char* dllFileName = gameModule.moduleFileName;

#if _LDK_DEBUG_
			// load a copy of the dll, so the original can be recompiled
			const char* dllCopyFileName = "ldare_game_copy.dll";
			if (!CopyFileA(dllFileName, dllCopyFileName, false))
			{
				LogError("Error copying game dll\n");
				return false;
			}
			dllFileName = dllCopyFileName;
			FILETIME originalDllWriteTime = Win32_getFileWriteTime(gameModule.moduleFileName);
#endif //_LDK_DEBUG_

			if ((gameModule.hGameModule = LoadLibraryA(dllFileName)))
			{
				gameModule.gameModuleWriteTime = originalDllWriteTime;
				gameModule.init = (gameInitFunc*)GetProcAddress(gameModule.hGameModule, "gameInit");
				gameModule.start = (gameStartFunc*)GetProcAddress(gameModule.hGameModule, "gameStart");
				gameModule.update = (gameUpdateFunc*)GetProcAddress(gameModule.hGameModule, "gameUpdate");
				gameModule.stop = (gameStopFunc*)GetProcAddress(gameModule.hGameModule, "gameStop");
			}
			else
			{
				LogError("Error loading game module\n");
				return false;
			}

			if (!(gameModule.init && gameModule.start && gameModule.update && gameModule.stop))
				return false;

			return true;
		}

		//---------------------------------------------------------------------------
		//  
		//  Audio structures and functions
		//
		//---------------------------------------------------------------------------

#define LDARE_MAX_AUDIO 32
		typedef decltype(&XAudio2Create) XAudio2CreateFunc;
		static IXAudio2* pXAudio2 = nullptr;
		static IXAudio2_7* pXAudio2_7 = nullptr; 

		//---------------------------------------------------------------------------
		// Initializes XInput
		//---------------------------------------------------------------------------
		static void Win32_initXInput()
		{
			char* xInputDllName = "xinput1_3.dll";
			HMODULE hXInput = LoadLibraryA(xInputDllName);
			if (!hXInput)
			{				
				xInputDllName = "xinput9_1_0.dll";
				hXInput = LoadLibraryA(xInputDllName);
			}

			if (!hXInput)
			{
				xInputDllName = "xinput1_1.dll";
				hXInput = LoadLibraryA(xInputDllName);
			}

			if (!hXInput)
			{
				LogError("could not initialize XInput. No suitable xinput dll found.");
				return;
			}

			LogInfo("XInput %s initialized.", xInputDllName);
			//get xinput function pointers
			XInputGetState = (XInputGetStateFunc*) GetProcAddress(hXInput, "XInputGetState");
			if (!XInputGetState) XInputGetState = XInputGetStateStub;
		}

		// represents an audio buffer bound to a source voice
		struct BoundAudio
		{
			XAUDIO2_BUFFER buffer;
			IXAudio2SourceVoice* voice;
		};

		static BoundAudio _boundBufferList[LDARE_MAX_AUDIO];
		static uint32 _boundBufferCount = 0;

		//---------------------------------------------------------------------------
		// Initializes XAudio2
		//--------------------------------------------------------------------------- 
		static void Win32_initXAudio()
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

				if (FAILED(pXAudio2_7))
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

		//---------------------------------------------------------------------------
		// Plays an audio buffer
		// Returns the created buffer id
		//---------------------------------------------------------------------------
		static uint32 createAudioBuffer(void* fmt, uint32 fmtSize, void* data, uint32 dataSize)
		{
			BoundAudio* audio = nullptr;
			uint32 audioId = _boundBufferCount;

			if (_boundBufferCount < LDARE_MAX_AUDIO)
			{
				// Get an audio buffer from the list
				audio = &(_boundBufferList[audioId]);
				_boundBufferCount++;
			}
			else
			{
				return -1;
			}

			// set format
			WAVEFORMATEXTENSIBLE wfx = *((WAVEFORMATEXTENSIBLE*) fmt);
			// set data
			BYTE *pDataBuffer = (BYTE*) data;

			// set XAUDIO2 instructions on what and how to play
			audio->buffer.AudioBytes = dataSize;
			audio->buffer.pAudioData = (BYTE*) data;
			audio->buffer.Flags = XAUDIO2_END_OF_STREAM;

			HRESULT hr = 0;
			//TODO: figure out how to use one single struct for both modern and legacy XAudio
			if (pXAudio2_7 != nullptr)
			{
				hr = pXAudio2_7->CreateSourceVoice(&audio->voice, (WAVEFORMATEX*)&wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO,nullptr, nullptr);
			}
			else
			{
				hr = pXAudio2->CreateSourceVoice(&audio->voice, (WAVEFORMATEX*)&wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO,nullptr, nullptr);
			}
			if (FAILED(hr))
			{
				LogError("Error creating source voice");
			}
			return audioId; 
		}

		//---------------------------------------------------------------------------
		// Plays an audio buffer
		//---------------------------------------------------------------------------
		void playAudio(uint32 audioBufferId)
		{
			if (_boundBufferCount >= LDARE_MAX_AUDIO || _boundBufferCount <= 0)
				return;

			BoundAudio* audio = &(_boundBufferList[audioBufferId]);
			HRESULT hr = audio->voice->SubmitSourceBuffer(&audio->buffer);

			if (FAILED(hr))
			{
				LogError("Error %x submitting audio buffer", hr);
			}

			hr = audio->voice->Start(0);
			if (FAILED(hr))
			{
				LogError("Error %x playing audio", hr);
			}
		}

	}	// platform namespace
} 	// ldare namespace
