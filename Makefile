# source paths
LDARESDK=src\sdk
LDARESRC=src\win32\win32_ldare.cpp
GAMESRC=game\test_game.cpp

#build targets
TARGET=ldare.exe
OUTDIR=build
LDARE_GAME=$(OUTDIR)\ldare_game.dll

#DEBUG OPTIONS
LIBS=user32.lib gdi32.lib Opengl32.lib
DEBUG_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDARESDK) /D "WIN32" /D "DEBUG" /Zi 
DEBUG_LINK_OPTIONS=/link /subsystem:console $(LIBS)

#RELEASE OPTIONS
RELEASE_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDARESDK) /D "WIN32" 
RELEASE_LINK_OPTIONS=/link /subsystem:windows $(LIBS)

CFLAGS=$(DEBUG_COMPILE_OPTIONS)
LINKFLAGS=$(DEBUG_LINK_OPTIONS)

.PHONY: game engine assets

all: engine game assets

game: $(LDARE_GAME) assets

$(LDARE_GAME): $(GAMESRC) 
	@echo Building game dll...
	cl $(GAMESRC) /Fo$(OUTDIR)\ /Fe$(LDARE_GAME) /LD $(CFLAGS) /link /subsystem:windows /EXPORT:gameInit /EXPORT:gameStart /EXPORT:gameUpdate /EXPORT:gameStop /PDB:$(OUTDIR)\ldare_game_%random%.pdb
	@echo copying game assets

assets:
	@echo copying game assets ...
	xcopy game\assets $(OUTDIR)\assets /Y /I /E /F
	@echo copying standard engine assets ...
	xcopy assets $(OUTDIR)\assets /Y /I /E /F

engine: $(LDARE_GAME) $(LDARE_CORE) assets
	@echo Building ldare engine...
cl $(LDARESRC) /Fe$(OUTDIR)\$(TARGET) /Fo$(OUTDIR)\ $(CFLAGS) /link /subsystem:console $(LIBS)

tool: src\win32\tools\makefont.cpp
	cl src\win32\tools\makefont.cpp /Fe$(OUTDIR)\makefont.exe /Fo$(OUTDIR)\  $(CFLAGS) $(LINKFLAGS)


clean:
	del /S /Q .\$(OUTDIR)\*
	rd /S /Q .\$(OUTDIR)\assets
