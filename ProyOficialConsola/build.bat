@echo off
rem Los comodines (core\*.cpp) no los expande cmd ni este g++ de MinGW; se listan los archivos.
g++ -std=c++11 -Wall -o HillConsola.exe main.cpp core\AlfabetoHill.cpp core\GaussJordan.cpp core\HillCipher.cpp core\MatrizMod26.cpp
if errorlevel 1 (echo Error de compilacion) else (echo Compilado: HillConsola.exe)
