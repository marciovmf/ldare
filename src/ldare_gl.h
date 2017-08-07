#ifndef __LDARE_GL__
#define __LDARE_GL__

#include "../GL/glcorearb.h"
#include "../GL/wglext.h"

#ifdef DECLARE_GL_POINTER
# define EXTERN
#else
# define EXTERN extern
#endif

extern "C"
{
	// OpenGL function pointers
	EXTERN PFNGLCLEARPROC glClear;
	EXTERN PFNGLCLEARCOLORPROC glClearColor;
	EXTERN PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	EXTERN PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	EXTERN PFNGLGENBUFFERSPROC glGenBuffers;
	EXTERN PFNGLBINDBUFFERPROC glBindBuffer;
	EXTERN PFNGLDELETEBUFFERSPROC glDeleteBuffers;
	EXTERN PFNGLBUFFERSUBDATAPROC glBufferSubData;
	EXTERN PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
	EXTERN PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	EXTERN PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttributeArray;
	EXTERN PFNGLGETERRORPROC glGetError;
}

namespace ldare 
{
	namespace platform
	{
		void* getGlFunctionPointer(const char* glFunctionName);
	}
}
#endif // _LDARE_GL__
