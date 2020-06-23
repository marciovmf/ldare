GAME_NAME=tof
SHELL=cmd

# paths
LDKSDK=..\..\src\include
OUTDIR=..\..\build

GAME_OUTDIR=$(OUTDIR)\$(GAME_NAME)

#build targets
LDK_GAME=$(GAME_OUTDIR)\ldk_game.dll
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

tof: tof_main.cpp
	IF NOT EXIST "$(GAME_OUTDIR)" @mkdir "$(GAME_OUTDIR)"
	cl tof_main.cpp /Fo$(GAME_OUTDIR) /Fe$(LDK_GAME) /LD $(CFLAGS) /link /subsystem:windows /PDB:$(OUTDIR)\$(GAME_NAME)%random%.pdb $(OUTDIR)/ldk.lib
	xcopy assets $(GAME_OUTDIR)\assets /S /Y /I /E /F > nul
	xcopy ..\..\assets $(GAME_OUTDIR)\assets /S /Y /I /E /F > nul
	copy /Y $(OUTDIR)\ldk_editor.exe $(GAME_OUTDIR)\$(GAME_NAME).exe
	copy /Y $(OUTDIR)\ldk.dll $(GAME_OUTDIR)

run:
	$(GAME_OUTDIR)\$(GAME_NAME).exe

clean:
	IF EXIST $(GAME_OUTDIR) rd /S /Q $(GAME_OUTDIR) > nul
