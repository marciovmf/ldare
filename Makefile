
TARGET=ldare.exe
OUTDIR=build
LIBS=user32.lib gdi32.lib Opengl32.lib
LDARESDK=src\sdk

DEBUG_OPTIONS=/Zi /nologo /EHsc /D "DEBUG" /MT

all: ldared

ldare:
	cl src\win32\win32_ldare.cpp /nologo /MT /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ /link /subsystem:windows $(LIBS)

ldared:
	cl src\win32\win32_ldare.cpp game\test_game.cpp /I$(LDARESDK) $(DEBUG_OPTIONS) /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ /link /subsystem:console $(LIBS)

clean:
	del /S /Q .\$(OUTDIR)\*
