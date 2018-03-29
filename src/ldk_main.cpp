
#include <ldk/ldk.h>
#include "ldk_platform.h"

uint32 ldkMain(uint32 argc, char** argv)
{
	if (! ldk::platform::initialize())
	{
		LogError("Error initializing platform layer");
		return LDK_EXIT_FAIL;
	}

	ldk::platform::LDKWindow* window = ldk::platform::createWindow(nullptr, "LDK", nullptr);

	if (!window)
	{

		LogError("Error creating main window");
		return LDK_EXIT_FAIL;
	}

	while (!ldk::platform::windowShouldClose(window))
	{
		ldk::platform::pollEvents();
	}

	return LDK_EXIT_SUCCESS;
}

#if _LDK_WINDOWS_
#include <windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//TODO: Handle command line arguments
	return ldkMain(0, nullptr);
}

#if _LDK_DEBUG_
#include <tchar.h>
int _tmain(int argc, _TCHAR** argv)
{
	//TODO: parse command line here and pass it to winmain
	return WinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOW);
}
#endif // _LDK_DEBUG_

#else // _LDK_WINDOWS_
int main(int argc, char** argv)
{
	return ldkMain(argc, argv);
}
#endif

