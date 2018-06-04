# source paths
LDKSDK=src\include
LDKSRC=src\ldk.cpp
EDITORSRC=src\ldk_main.cpp
GAMESRC=game\game.cpp

#build targets
OUTDIR=build
LDK_GAME=$(OUTDIR)\ldk_game.dll
LDK_PLATFORM=$(OUTDIR)\platform.obj

#DEBUG OPTIONS
LIBS=user32.lib gdi32.lib Opengl32.lib Msimg32.lib Ole32.lib
DEBUG_DEFINES=/D "_LDK_WINDOWS_" /D "_LDK_DEBUG_" 
DEBUG_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDKSDK) $(DEBUG_DEFINES) /Zi
DEBUG_LINK_OPTIONS=/link /subsystem:console $(LIBS)

#RELEASE OPTIONS
RELEASE_DEFINES=/D "_LDK_WINDOWS_" /D "_LDK_EDITOR_" 
RELEASE_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDKSDK) $(RELEASE_DEFINES) /Zi
RELEASE_LINK_OPTIONS=/link /subsystem:windows $(LIBS)

CFLAGS=$(DEBUG_COMPILE_OPTIONS)
LINKFLAGS=$(DEBUG_LINK_OPTIONS)

.PHONY: game ldk assets

all: outdirfolder platform ldk game assets

game: assets

game:
	@echo Building game dll...
	cl $(GAMESRC) /Fo$(OUTDIR)\ /Fe$(LDK_GAME) /LD $(CFLAGS) /link /subsystem:windows /EXPORT:gameInit /EXPORT:gameStart /EXPORT:gameUpdate /EXPORT:gameStop /PDB:$(OUTDIR)\ldare_game_%random%.pdb
	@echo copying game assets

assets:
	@echo copying game assets ...
	xcopy game\assets $(OUTDIR)\assets /Y /I /E /F > NUL
	@echo copying standard assets ...
	xcopy assets $(OUTDIR)\assets /Y /I /E /F > NUL

platform:
	@echo buiding platform
	cl src\win32\ldk_platform_win32.cpp /c /Fo$(LDK_PLATFORM) $(CFLAGS)

ldk: platform
	@echo Building ldk...
	cl src/ldk.cpp /Fo$(OUTDIR)\ /Fe$(OUTDIR)\ldk.dll /LD $(CFLAGS) /link /DLL /subsystem:windows $(LIBS)  $(LDK_PLATFORM)

editor: 
	cl src\ldk_editor.cpp /Fe$(OUTDIR)\ldk.exe /Fo$(OUTDIR)\ $(CFLAGS) $(LINKFLAGS) $(OUTDIR)/ldk.dll

tool: src\win32\tools\ldk_tool_font.cpp
	cl src\win32\tools\ldk_tool_font.cpp /Fe$(OUTDIR)\makefont.exe /Fo$(OUTDIR)\ $(CFLAGS) $(LINKFLAGS)

outdirfolder:
	IF NOT EXIST "$(OUTDIR)" mkdir $(OUTDIR)
clean:
	@IF EXIST "$(OUTDIR)" del /S /Q .\$(OUTDIR)\* > NUL
	@IF EXIST "$(OUTDIR)\assets" rd /S /Q .\$(OUTDIR)\assets > NUL
