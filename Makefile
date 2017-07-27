# source paths
LDARESDK=src\sdk
LDARESRC=src\win32\win32_ldare.cpp
GAMESRC=game\test_game.cpp

#build targets
TARGET=ldare.exe
OUTDIR=build
LDARE_GAME=$(OUTDIR)\ldare_game.obj
LDARE_CORE=$(OUTDIR)\ldare_core.obj

#DEBUG OPTIONS
LIBS=user32.lib gdi32.lib Opengl32.lib
DEBUG_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDARESDK) /D "WIN32" /D "DEBUG" /Zi 
DEBUG_LINK_OPTIONS=/link /subsystem:console $(LIBS)

#RELEASE OPTIONS
RELEASE_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDARESDK) /D "WIN32" 
RELEASE_LINK_OPTIONS=/link /subsystem:windows $(LIBS)

CFLAGS=$(DEBUG_COMPILE_OPTIONS)
LINKFLAGS=$(DEBUG_LINK_OPTIONS)

all: ldare_game ldare_core
	cl $(LDARE_CORE) $(LDARE_GAME) /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ $(CFLAGS) $(LINKFLAGS)

ldare_game: $(GAMESRC)
	cl /c $(GAMESRC) /Fo$(LDARE_GAME) $(CFLAGS) 

ldare_core: $(LDARESRC)
	cl /c $(LDARESRC) /Fo$(LDARE_CORE) $(CFLAGS) /D "LDARE_ENGINE" 

clean:
	del /S /Q .\$(OUTDIR)\*
