/**
 * win32_platform.h
 * Win32 implementation for ldare platform functions
 */
#include <windows.h>
namespace ldare 
{
	namespace platform 
	{
		void* memoryAlloc(size_t size)
		{
			ASSERT(size>0, "allocation size must be greater than zero");
			void* mem =
				VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (mem==0) { LogError("Error allocating memory"); }
			return mem;
		}

		void* getGlFunctionPointer(const char* functionName)
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

		void* loadFileToBuffer(const char8* fileName, size_t* bufferSize)
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
	}	// platform namespace
} 	// ldare namespace
