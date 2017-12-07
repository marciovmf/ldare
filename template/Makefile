#---------------------------------------------------------------------------
# Set these values
#---------------------------------------------------------------------------
GAME_NAME=
GAME_PROJECT_DIR=
LDARE_HOME=

#---------------------------------------------------------------------------
# GAME folders
#---------------------------------------------------------------------------
	GAME_SRC_DIR=$(GAME_PROJECT_DIR)\src
	GAME_BUILD_DIR=$(GAME_PROJECT_DIR)\build
	GAME_ASSET_DIR=$(GAME_PROJECT_DIR)\assets
	GAME_SOURCES=$(GAME_SRC_DIR)\game.cpp

#---------------------------------------------------------------------------
# LDARE folders
#---------------------------------------------------------------------------
LDARE_BIN=$(LDARE_HOME)\build\ldare.exe
LDARESDK=$(LDARE_HOME)\src\sdk

#---------------------------------------------------------------------------
# BUILD settings
#---------------------------------------------------------------------------
#build target - DO NOT CHANGE THIS
GAME_DLL=$(GAME_BUILD_DIR)\ldare_game.dll
#DEBUG OPTIONS
DEBUG_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDARESDK) /D "WIN32" /D "DEBUG" /Zi 
#RELEASE OPTIONS
RELEASE_COMPILE_OPTIONS=/nologo /EHsc /MT /I$(LDARESDK) /D "WIN32" 

#CFLAGS=$(RELEASE_COMPILE_OPTIONS)
CFLAGS=$(DEBUG_COMPILE_OPTIONS)

.PHONY: game assets clean

all: game assets

game: $(GAME_SOURCES)
	@echo Building game dll...
	@del /q /s $(GAME_BUILD_DIR)\*.pdb 2>nul
	cl $(GAME_SOURCES) /Fo$(GAME_BUILD_DIR)\ /Fe$(GAME_DLL) /LD $(CFLAGS) /link /subsystem:windows /EXPORT:gameInit /EXPORT:gameStart /EXPORT:gameUpdate /EXPORT:gameStop /PDB:$(GAME_BUILD_DIR)\ldare_game_%random%.pdb
	@copy $(LDARE_BIN) $(GAME_BUILD_DIR)\$(GAME_NAME).exe

assets:
	@echo copying game assets ...
	@xcopy $(GAME_ASSET_DIR) $(GAME_BUILD_DIR)\assets /Y /I /E /F
	@echo copying standard engine assets ...
	@xcopy $(LDARE_HOME)\assets $(GAME_BUILD_DIR)\assets /Y /I /E /F

clean:
	@del /S /Q $(GAME_BUILD_DIR)\*
	@del /S /Q $(GAME_BUILD_DIR)\assets
















































