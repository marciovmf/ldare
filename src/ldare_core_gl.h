#ifndef _LDARE_COREGL_
#define _LDARE_COREGL_

#include "../GL/glcorearb.h"
#include "../GL/wglext.h"

// OpenGL function pointers
PFNGLCLEARPROC glClear;
PFNGLCLEARCOLORPROC glClearColor;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
#endif // _LDARE_COREGL_

