#include <windows.h>
#include <tchar.h>

int CALLBACK WinMain(
   HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   LPSTR     lpCmdLine,
   int       nCmdShow
)
{
 MessageBox(0, "Teste","Teste", MB_OK);
 return 0;
}

#ifdef DEBUG
#include <iostream>
int _tmain(int argc, _TCHAR** argv)
{
	std::cout << "Initializing ldare ..." << std::endl;
	return WinMain(GetModuleHandle(NULL), NULL, NULL,SW_SHOW);
}
#endif
