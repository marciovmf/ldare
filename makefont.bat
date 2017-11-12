@echo off
cd assets
..\build\makefont.exe %1 %2 60 512 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789{[(<>)]}|,.:;+-_=\/?!%$#@'\""
cd ..

