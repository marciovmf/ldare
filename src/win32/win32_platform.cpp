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
			void* mem =
				VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#ifdef DEBUG
			ASSERT(mem!=0);
#endif
			if (!mem) { LogError("Error allocaing memory"); }
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

	}	// platform namespace
} 	// ldare namespace
