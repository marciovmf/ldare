@echo off
rem ----------------------------------------------------------------------------------
rem This is a small utility for extracting TODO: and TODO(xxx) comments from code
rem so I can track and eventually fix them.
rem - @marciovmf
rem ----------------------------------------------------------------------------------

echo --------------------------------TODO LIST------------------------------------
echo        Hey Marcio, If this list is not empty, you have work to do!
echo -----------------------------------------------------------------------------
findstr /S /N /R /A:0A /C:"//TODO[:|\(.*\):]" *.h *.cpp *.frag *.vert > TODO.tmp

for /F "delims=: tokens=1,2,3,4 " %%a in (TODO.tmp) do (
		echo %%a^|%%b^| %%d 

)

