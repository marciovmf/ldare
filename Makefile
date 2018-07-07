# paths
LDKSDK=src\include
GAMESRC=game\game.cpp
OUTDIR=build

#build targets
LDK_GAME=$(OUTDIR)\ldk_game.dll
LDK_ENGINE=$(OUTDIR)\ldk.dll $(OUTDIR)\ldk.lib
LDK_EDITOR=$(OUTDIR)\ldk_editor.exe

#DEBUG OPTIONS
LIBS=user32.lib gdi32.lib Opengl32.lib Msimg32.lib Ole32.lib
DEBUG_DEFINES=/D "_LDK_DEBUG_" 
DEBUG_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDKSDK) $(DEBUG_DEFINES) /Zi
DEBUG_LINK_OPTIONS=/link /subsystem:console $(LIBS)

#RELEASE OPTIONS
RELEASE_DEFINES=
RELEASE_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDKSDK) $(RELEASE_DEFINES) /Zi
RELEASE_LINK_OPTIONS=/link /subsystem:windows $(LIBS)

CFLAGS=$(DEBUG_COMPILE_OPTIONS)
LINKFLAGS=$(DEBUG_LINK_OPTIONS)

.PHONY: outdirfolder clean assets

all: outdirfolder assets game

ldk: $(LDK_ENGINE)

editor: $(LDK_EDITOR)

game: $(LDK_GAME)

$(LDK_GAME): $(LDK_EDITOR) game/*.cpp
	@echo Building game dll...
	cl game\game.cpp /Fo$(OUTDIR)\ /Fe$(LDK_GAME) /LD $(CFLAGS) /link /subsystem:windows /PDB:$(OUTDIR)\ldare_game_%random%.pdb $(OUTDIR)/ldk.lib


$(LDK_ENGINE): src/*.cpp src/*.h 
	@echo Building ldk...
	cl src\ldk.cpp /Fo$(OUTDIR)\ /Fe$(OUTDIR)\ldk.dll /LD $(CFLAGS) /D "_LDK_ENGINE_" /link /DLL /subsystem:windows $(LIBS)

$(LDK_EDITOR): $(LDK_ENGINE) src/ldk_editor.cpp
	cl src\ldk_editor.cpp /Fe$(OUTDIR)/ $(CFLAGS) $(LINKFLAGS) $(OUTDIR)/ldk.lib
	@echo SUCCESS

tool: src\win32\tools\ldk_tool_font.cpp
	cl src\win32\tools\ldk_tool_font.cpp /Fe$(OUTDIR)\makefont.exe /Fo$(OUTDIR)\ $(CFLAGS) $(LINKFLAGS)

assets:
	@echo copying assets ...
	@xcopy game\assets $(OUTDIR)\assets /Y /I /E /F > NUL
	@xcopy assets $(OUTDIR)\assets /Y /I /E /F > NUL
	@copy /Y ldk.cfg $(OUTDIR) > NUL

outdirfolder:
	@IF NOT EXIST "$(OUTDIR)" mkdir $(OUTDIR)

clean:
	@IF EXIST "$(OUTDIR)" del /S /Q .\$(OUTDIR)\* > NUL
	@IF EXIST "$(OUTDIR)\assets" rd /S /Q .\$(OUTDIR)\assets > NUL
