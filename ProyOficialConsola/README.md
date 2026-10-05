# HillConsola

Cifrado de Hill (mod 26, alfabeto oficial A=5, B=23, ...) en consola.
Autocontenido: usa solo la libreria estandar de C++ (`core/` + `main.cpp`).

## Compilar con g++
En Windows (cmd):
```
build.bat
```
que equivale a (se listan los archivos porque cmd/MinGW no expanden `core\*.cpp`):
```
g++ -std=c++11 -Wall -o HillConsola.exe main.cpp core\AlfabetoHill.cpp core\GaussJordan.cpp core\HillCipher.cpp core\MatrizMod26.cpp
```
En Git Bash, Linux o macOS basta: `g++ -std=c++11 -Wall -o HillConsola main.cpp core/*.cpp`

## Compilar con Dev-C++
1. Archivo > Nuevo > Proyecto > **Console Application** (C++).
2. Agregar `main.cpp` y todos los `core/*.cpp` al proyecto.
3. Proyecto > Opciones del proyecto > Parametros > C++ compiler: `-std=c++11`.
4. Compilar y ejecutar (F11).

## Uso
Ejecutar `HillConsola.exe` y elegir una opcion del menu (6 = pruebas automaticas).
