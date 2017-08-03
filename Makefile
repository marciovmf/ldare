# source paths
LDARESDK=src\sdk
LDARESRC=src\win32\win32_ldare.cpp
GAMESRC=game\test_game.cpp
GLRENDERERSRC=src\ldare_renderer_gl.cpp

#build targets
TARGET=ldare.exe
OUTDIR=build
LDARE_GAME=$(OUTDIR)\ldare_game.obj
LDARE_CORE=$(OUTDIR)\ldare_core.obj
LDARE_GL_RENDERER=$(OUTDIR)\ldare_gl_renderer.obj

#DEBUG OPTIONS
LIBS=user32.lib gdi32.lib Opengl32.lib
DEBUG_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDARESDK) /D "WIN32" /D "DEBUG" /Zi 
DEBUG_LINK_OPTIONS=/link /subsystem:console $(LIBS)

#RELEASE OPTIONS
RELEASE_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDARESDK) /D "WIN32" 
RELEASE_LINK_OPTIONS=/link /subsystem:windows $(LIBS)

CFLAGS=$(DEBUG_COMPILE_OPTIONS)
LINKFLAGS=$(DEBUG_LINK_OPTIONS)

.PHONY: game engine gl_renderer

all: $(TARGET)

game: $(LDARE_GAME)
gl_renderer: $(LDARE_GL_RENDERER)
engine:	$(LDARE_CORE)


$(LDARE_GAME): $(GAMESRC)
	cl /c $(GAMESRC) /Fo$(LDARE_GAME) $(CFLAGS) 

$(LDARE_GL_RENDERER): $(GLRENDERERSRC)
	cl /c $(GLRENDERERSRC) /Fo$(LDARE_GL_RENDERER) $(CFLAGS) 

$(LDARE_CORE): $(LDARESRC)
	cl /c $(LDARESRC) /Fo$(LDARE_CORE) $(CFLAGS) /D "LDARE_ENGINE" 

$(TARGET): $(LDARE_GAME) $(LDARE_CORE) $(LDARE_GL_RENDERER)
	cl $(LDARE_CORE) $(LDARE_GAME) $(LDARE_GL_RENDERER) /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ $(CFLAGS) $(LINKFLAGS)

clean:
	del /S /Q .\$(OUTDIR)\*
