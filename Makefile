
SHELL=cmd

LDKSDK=src\include
OUTDIR=build

#build targets
LDK_GAME=$(OUTDIR)\ldk_game.dll
LDK_ENGINE=$(OUTDIR)\ldk.dll
LDK_BAKE_TOOL=$(OUTDIR)\bake.exe

LIBS=user32.lib gdi32.lib Opengl32.lib Msimg32.lib Ole32.lib

#DEBUG OPTIONS
DEBUG_DEFINES=/D "_LDK_DEBUG_" 
DEBUG_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDKSDK) $(DEBUG_DEFINES) /Zi
DEBUG_LINK_OPTIONS=/link /subsystem:console $(LIBS)

#RELEASE OPTIONS
RELEASE_DEFINES=
RELEASE_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDKSDK) $(RELEASE_DEFINES)
RELEASE_LINK_OPTIONS=/link /subsystem:windows $(LIBS)

CFLAGS=$(DEBUG_COMPILE_OPTIONS)
LINKFLAGS=$(DEBUG_LINK_OPTIONS)

VERSION=0.1
PACKAGE_NAME=$(OUTDIR)\ldk_$(VERSION)

.PHONY: clean all tools package

all: ldk game

ldk: $(LDK_ENGINE)

game: ldk $(LDK_GAME)

tools: $(LDK_BAKE_TOOL)

$(LDK_ENGINE): src/*.cpp src/*.h src/include/ldk/*.h
	@IF NOT EXIST "$(OUTDIR)" mkdir $(OUTDIR) 2> NUL
	@echo === Building ldk...
	cl src\ldk.cpp /Fo$(OUTDIR)\ /Fe$(OUTDIR)\ldk.dll /LD $(CFLAGS) /D "_LDK_ENGINE_" /link /DLL /subsystem:windows $(LIBS)
	cl src\ldk_editor.cpp /Fe$(OUTDIR)/ $(CFLAGS) $(LINKFLAGS) $(OUTDIR)/ldk.lib
	@echo === Copying standard assets...
	xcopy assets $(OUTDIR)\assets /Y /I /E /F > nul
	xcopy assets\standard $(OUTDIR)\assets\standard /Y /I /E /F > nul
	copy /Y ldk_game.cfg $(OUTDIR) > nul

$(LDK_BAKE_TOOL): src/tools/*.cpp src/win32/tools/*.cpp
	@IF NOT EXIST "$(OUTDIR)" mkdir $(OUTDIR) 2> NUL
	cl src\win32\tools\ldk_tool_font.cpp /Fe$(OUTDIR)\makefont.exe /Fo$(OUTDIR)\ $(CFLAGS) $(LINKFLAGS)
	cl src\tools\ldk_bakeTool.cpp /Fe$(OUTDIR)\bake.exe /Fo$(OUTDIR)\ $(CFLAGS) $(LINKFLAGS) $(OUTDIR)\ldk.lib

$(LDK_GAME): game/mo/*.cpp
	@echo === Building game... 
	del /F /Q $(OUTDIR)\ldk_game*.pdb 2> NUL
	cl game\mo\game.cpp /Fo$(OUTDIR)\ /Fe$(LDK_GAME) /LD $(CFLAGS) /link /subsystem:windows /PDB:$(OUTDIR)\ldk_game_%random%.pdb $(OUTDIR)/ldk.lib
	@xcopy game\mo\assets $(OUTDIR)\assets /Y /S /I /E /F > nul

tof: $(LDK_ENGINE) game/tof/*.cpp
	make --directory game/tof

match3: $(LDK_ENGINE) game/match3/*.cpp
	make --directory game/match3

package:
	@IF NOT EXIST "$(PACKAGE_NAME)" mkdir "$(PACKAGE_NAME)"
	@IF NOT EXIST "$(PACKAGE_NAME)\build" mkdir "$(PACKAGE_NAME)\build"
	@IF NOT EXIST "$(PACKAGE_NAME)\ldk\lib" mkdir "$(PACKAGE_NAME)\ldk\lib"
	@IF NOT EXIST "$(PACKAGE_NAME)\ldk\include" mkdir "$(PACKAGE_NAME)\ldk\include"
	@xcopy assets $(PACKAGE_NAME)\assets /Y /I /E /F
	xcopy "$(LDKSDK)" "$(PACKAGE_NAME)\ldk\include" /E /F /Y /I 
	copy /Y "ldk_game.cfg" "$(PACKAGE_NAME)\build\ldk_game.cfg"
	copy /Y "$(OUTDIR)\ldk_editor.exe" "$(PACKAGE_NAME)\build\ldk_editor.exe"
	copy /Y "$(OUTDIR)\ldk.dll" "$(PACKAGE_NAME)\build\ldk.dll"
	copy /Y "$(OUTDIR)\ldk.lib" "$(PACKAGE_NAME)\ldk\lib\ldk.lib"
	copy /Y "$(OUTDIR)\ldk.pdb" "$(PACKAGE_NAME)\ldk\lib\pdb.lib"

clean:
	@IF EXIST "$(OUTDIR)" rd /S /Q $(OUTDIR) 2> NUL
