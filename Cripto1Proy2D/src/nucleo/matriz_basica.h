// matriz_basica.h
// Contiene: construccion de matrices (crearMatriz, identidad), transpuesta,
//           producto, determinante (por cofactores) y comparacion de matrices.
// No hace: no resuelve sistemas ni calcula inversas (ver gauss_jordan.h).
// Lo usa: hill.cpp, src/ataque/*, src/interfaz/main.cpp, src/pruebas/tests.cpp.
#ifndef MATRIZ_BASICA_H
#define MATRIZ_BASICA_H

#include "matriz.h"

// Matriz de ceros de tamaño filas x columnas.
Matriz crearMatriz(int filas, int columnas);

// Matriz identidad n x n.
Matriz identidad(int n);

// Transpuesta: intercambia filas por columnas.
Matriz transpuesta(const Matriz& A);

// Producto A * B módulo m (A es f x k, B es k x c).
Matriz multiplicar(const Matriz& A, const Matriz& B, int m);

// Determinante módulo m por expansión de cofactores (primera fila).
int determinante(const Matriz& A, int m);

// true si las dos matrices tienen el mismo tamaño y los mismos valores.
bool matricesIguales(const Matriz& A, const Matriz& B);

#endif
