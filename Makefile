
TARGET=ldare.exe
OUTDIR=build
LIBS=user32.lib gdi32.lib

all: ldared

ldare:
	cl src\win32\win32_ldare.cpp /nologo /MT /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ /link /subsystem:windows $(LIBS)

ldared:
	cl src\win32\win32_ldare.cpp /Zi /nologo /EHsc /D "DEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /MT /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ /link /subsystem:console $(LIBS)

