#ifndef _LDARE_COREGL_
#define _LDARE_COREGL_

#include "../GL/glcorearb.h"
#include "../GL/wglext.h"

#ifdef DECLARE_EXTERN_GL
#	define EXTERN extern
#else
#	define EXTERN
#endif

EXTERN PFNGLCLEARPROC glClear;
EXTERN PFNGLCLEARCOLORPROC glClearColor;
EXTERN PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
EXTERN PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
#endif // _LDARE_COREGL_

