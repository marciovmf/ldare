# source paths
LDKSDK=src\include
LDKSRC=src\ldk_main.cpp
GAMESRC=game\game.cpp

#build targets
TARGET=ldk.exe
OUTDIR=build
LDK_GAME=$(OUTDIR)\ldk_game.dll

#DEBUG OPTIONS
LIBS=user32.lib gdi32.lib Opengl32.lib Msimg32.lib Ole32.lib
DEBUG_DEFINES=/D "_LDK_WINDOWS_" /D "_LDK_DEBUG_" /D "_LDK_EDITOR_" 
DEBUG_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDKSDK) $(DEBUG_DEFINES) /Zi
DEBUG_LINK_OPTIONS=/link /subsystem:console $(LIBS)

#RELEASE OPTIONS
RELEASE_DEFINES=/D "_LDK_WINDOWS_" /D "_LDK_EDITOR_" 
RELEASE_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDKSDK) $(RELEASE_DEFINES) /Zi
RELEASE_LINK_OPTIONS=/link /subsystem:windows $(LIBS)

CFLAGS=$(DEBUG_COMPILE_OPTIONS)
LINKFLAGS=$(DEBUG_LINK_OPTIONS)

.PHONY: game engine assets

all: platform engine game assets

game: assets

game:
	@echo Building game dll...
	cl $(GAMESRC) /Fo$(OUTDIR)\ /Fe$(LDK_GAME) /LD $(CFLAGS) /link /subsystem:windows /EXPORT:gameInit /EXPORT:gameStart /EXPORT:gameUpdate /EXPORT:gameStop /PDB:$(OUTDIR)\ldare_game_%random%.pdb
	@echo copying game assets

assets:
	@echo copying game assets ...
	xcopy game\assets $(OUTDIR)\assets /Y /I /E /F
	@echo copying standard engine assets ...
	xcopy assets $(OUTDIR)\assets /Y /I /E /F


platform:
	@echo buiding platform
	cl src\win32\ldk_platform_win32.cpp /c /Fo$(OUTDIR)\platform.obj $(CFLAGS)

engine: platform
	@echo Building ldare engine...
	cl $(LDKSRC) /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ $(CFLAGS) /link /subsystem:console $(LIBS) $(OUTDIR)\platform.obj

tool: src\win32\tools\ldare_tool_font.cpp
	cl src\win32\tools\ldare_tool_font.cpp /Fe$(OUTDIR)\makefont.exe /Fo$(OUTDIR)\  $(CFLAGS) $(LINKFLAGS)

clean:
	del /S /Q .\$(OUTDIR)\*
	rd /S /Q .\$(OUTDIR)\assets
