
TARGET=ldare.exe
OUTDIR=build
LIBS=user32.lib

ldare:
	cl src\win32\win32_ldare.cpp /nologo /MT /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ /link /subsystem:windows $(LIBS)

ldared:
	cl src\win32\win32_ldare.cpp /nologo /DDEBUG /MT /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ /link /subsystem:console $(LIBS)

