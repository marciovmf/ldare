@echo off
cd assets
 ..\build\makefont.exe %1 %2 40 512 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789{[(<>)]}|,.:;+-_=\/?!%$#@'\""
cd ..

