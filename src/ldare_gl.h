#ifndef __LDARE_COREGL__
#define ___LDARE_COREGL__
#include <ldare/ldare.h>
#include "../GL/glcorearb.h"
#include "../GL/wglext.h"

extern "C"
{
	// OpenGL function pointers
	LDARE_API PFNGLCLEARPROC glClear;
	LDARE_API PFNGLCLEARCOLORPROC glClearColor;
	LDARE_API PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	LDARE_API PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	LDARE_API PFNGLGENBUFFERSPROC glGenBuffers;
	LDARE_API PFNGLGETERRORPROC glGetError;
}
#endif // __LDARE_COREGL__
