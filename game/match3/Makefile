
# paths
LDKSDK=..\..\src\include
OUTDIR=..\..\build

#build targets
LDK_GAME=$(OUTDIR)\match3\ldk_game.dll
LDK_ENGINE=$(OUTDIR)\ldk.dll $(OUTDIR)\ldk.lib
LDK_EDITOR=$(OUTDIR)\ldk_editor.exe

#DEBUG OPTIONS
LIBS=user32.lib gdi32.lib Opengl32.lib Msimg32.lib Ole32.lib
DEBUG_DEFINES=/D "_LDK_DEBUG_" 
DEBUG_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDKSDK) $(DEBUG_DEFINES) /Zi
DEBUG_LINK_OPTIONS=/link /subsystem:console $(LIBS)

#RELEASE OPTIONS
RELEASE_DEFINES=
RELEASE_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDKSDK) $(RELEASE_DEFINES)
RELEASE_LINK_OPTIONS=/link /subsystem:windows $(LIBS)

CFLAGS=$(DEBUG_COMPILE_OPTIONS)
LINKFLAGS=$(DEBUG_LINK_OPTIONS)

match3: match3.cpp
	IF NOT EXIST "$(OUTDIR)\match3" @mkdir $(OUTDIR)\match3
	cl match3.cpp /Fo$(OUTDIR)\match3 /Fe$(LDK_GAME) /LD $(CFLAGS) /link /subsystem:windows /PDB:$(OUTDIR)\match3_%random%.pdb $(OUTDIR)/ldk.lib
	xcopy assets $(OUTDIR)\match3\assets /Y /I /E /F > nul
	copy /Y $(OUTDIR)\ldk_editor.exe $(OUTDIR)\match3\match3.exe
	copy /Y $(OUTDIR)\ldk.dll $(OUTDIR)\match3

run:
	$(OUTDIR)\match3\match3.exe

clean:
	IF EXIST $(OUTDIR)\match3 rd /S /Q $(OUTDIR)\match3 > nul
