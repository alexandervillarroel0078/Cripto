@echo off
REM Compila el proyecto con g++ (MinGW)
if not exist build mkdir build
g++ -std=c++17 -static -Wall -o build\hill.exe ^
    src\nucleo\modular.cpp ^
    src\nucleo\matriz_basica.cpp ^
    src\nucleo\gauss_jordan.cpp ^
    src\nucleo\hill.cpp ^
    src\ataque\ataque_directo.cpp ^
    src\ataque\sistema_modular.cpp ^
    src\ataque\inconsistencia.cpp ^
    src\ataque\combinar_tcr.cpp ^
    src\ataque\correcciones.cpp ^
    src\ataque\criptoanalisis.cpp ^
    src\interfaz\traza.cpp ^
    src\interfaz\entrada.cpp ^
    src\interfaz\menu.cpp ^
    src\pruebas\tests.cpp
if errorlevel 1 (
    echo Error de compilacion.
    exit /b 1
)
echo Compilado: build\hill.exe
