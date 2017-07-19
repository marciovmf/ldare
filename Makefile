
TARGET=ldare.exe
OUTDIR=build
LIBS=user32.lib gdi32.lib Opengl32.lib
LDARESDK=src\sdk

DEBUG_OPTIONS=/Zi /nologo /EHsc /D "DEBUG" /MT
GAMESRC=game\test_game.cpp

all: ldared 

ldare:
	cl /C src\win32\win32_ldare.cpp /nologo /MT /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ /link /subsystem:windows $(LIBS)

game.obj: $(GAMESRC)
	cl /c  $(GAMESRC) /I$(LDARESDK) /nologo /D "DEBUG" /EHsc /Fo$(OUTDIR)\game.obj

ldared: game.obj
	cl src\win32\win32_ldare.cpp $(OUTDIR)\game.obj /I$(LDARESDK) $(DEBUG_OPTIONS) /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ /link /subsystem:console $(LIBS)

clean:
	del /S /Q .\$(OUTDIR)\*
