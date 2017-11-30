@echo off
rem ----------------------------------------------------------------------------------
rem This is a small utility for extracting TODO: NOTE: and BUG: comments from code
rem so I can track and eventually fix them.
rem - Marcio
rem ----------------------------------------------------------------------------------

echo //TODO list > TODO.txt

findstr /S /N /L /A:0A /C:"//TODO:" *.h *.cpp *.frag *.vert > TODO.tmp

for /F "delims=: tokens=1,2,3,4 " %%a in (TODO.tmp) do (
		echo %%a: %%b - //TODO: %%d >> TODO.txt
)

echo. >> TODO.TXT
echo //NOTE list >> TODO.TXT
findstr /S /N /L /A:0A /C:"//NOTE:" *.h *.cpp *.frag *.vert > TODO.tmp

for /F "delims=: tokens=1,2,3,4 " %%a in (TODO.tmp) do (
		echo %%a: %%b - //NOTE: %%d >> TODO.txt
)

echo. >> TODO.TXT
echo //BUG list >> TODO.TXT
findstr /S /N /L /A:0A /C:"//BUG:" *.h *.cpp *.frag *.vert > TODO.tmp

for /F "delims=: tokens=1,2,3,4 " %%a in (TODO.tmp) do (
		echo %%a :%%b - //BUG: %%d >> TODO.txt
)

del TODO.tmp
type TODO.txt
