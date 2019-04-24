
SHELL=cmd

LDKSDK=src\include
OUTDIR=build

#build targets
LDK_GAME=$(OUTDIR)\ldk_game.dll
LDK_ENGINE=$(OUTDIR)\ldk.dll
LDK_MESH_TOOL=$(OUTDIR)\mesh.exe
LDK_FONT_TOOL=$(OUTDIR)\font.exe

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

.PHONY: outdirfolder gameassets clean all tools

all: ldk tools game

ldk: $(LDK_ENGINE)

game: ldk $(LDK_GAME)

tools: ldk $(LDK_MESH_TOOL) $(LDK_FONT_TOOL)

$(LDK_ENGINE): src/*.cpp src/*.h src/include/ldk/*.h
	@IF NOT EXIST "$(OUTDIR)" mkdir $(OUTDIR) 2> NUL
	@echo === Building ldk...
	cl src\ldk.cpp /Fo$(OUTDIR)\ /Fe$(OUTDIR)\ldk.dll /LD $(CFLAGS) /D "_LDK_ENGINE_" /link /DLL /subsystem:windows $(LIBS)
	cl src\ldk_editor.cpp /Fe$(OUTDIR)/ $(CFLAGS) $(LINKFLAGS) $(OUTDIR)/ldk.lib
	@echo === Copying standard assets...
	xcopy assets $(OUTDIR)\assets /Y /I /E /F > nul
	xcopy assets\standard $(OUTDIR)\assets\standard /Y /I /E /F > nul
	copy /Y ldk_game.cfg $(OUTDIR) > nul

$(LDK_MESH_TOOL): src/tools/mesh/*.cpp
	@echo === Building mesh tool... 
	@IF NOT EXIST "$(OUTDIR)\tools" mkdir $(OUTDIR)\tools 2> NUL
	cl src\tools\mesh\ldk_mesh_tool.cpp /Fe$(OUTDIR)\mesh.exe /Fo$(OUTDIR)\ $(CFLAGS) $(LINKFLAGS) $(OUTDIR)\ldk.lib

$(LDK_FONT_TOOL): src/tools/font/*.cpp
	@echo === Building font tool... 
	@IF NOT EXIST "$(OUTDIR)\tools" mkdir $(OUTDIR)\tools 2> NUL
	cl src\tools\font\ldk_font_tool.cpp /Fe$(OUTDIR)\font.exe /Fo$(OUTDIR)\ $(CFLAGS) $(LINKFLAGS) $(OUTDIR)\ldk.lib

$(LDK_GAME): game/mo/*.cpp
	@echo === Building game... 
	del /F /Q $(OUTDIR)\ldk_game*.pdb 2> NUL
	cl game\mo\game.cpp /Fo$(OUTDIR)\ /Fe$(LDK_GAME) /LD $(CFLAGS) /link /subsystem:windows /PDB:$(OUTDIR)\ldk_game_%random%.pdb $(OUTDIR)/ldk.lib
	move "$(OUTDIR)\ldk_editor.exe" "$(OUTDIR)\mo.exe"
	@xcopy game\mo\assets $(OUTDIR)\assets /Y /I /E /F > nul

package:
	@IF NOT EXIST "$(OUTDIR)\ldk" mkdir "$(OUTDIR)\ldk"
	@IF NOT EXIST "$(OUTDIR)\ldk\lib" mkdir "$(OUTDIR)\ldk\lib"
	@IF NOT EXIST "$(OUTDIR)\ldk\include" mkdir "$(OUTDIR)\ldk\include"
	@xcopy assets $(OUTDIR)\ldk\assets /Y /I /E /F
	xcopy "$(LDKSDK)" "$(OUTDIR)\ldk\include" /E /F /Y /I 
	copy /Y "ldk_game.cfg" "$(OUTDIR)\ldk\ldk_game.cfg"
	copy /Y "$(OUTDIR)\*.exe" "$(OUTDIR)\ldk\"
	copy /Y "$(OUTDIR)\*.dll" "$(OUTDIR)\ldk\"
	copy /Y "$(OUTDIR)\*.lib" "$(OUTDIR)\ldk\lib\"

clean:
	@IF EXIST "$(OUTDIR)" rd /S /Q $(OUTDIR) 2> NUL
