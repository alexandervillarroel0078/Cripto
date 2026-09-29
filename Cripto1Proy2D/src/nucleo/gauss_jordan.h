// gauss_jordan.h
// Contiene: eliminacion de Gauss-Jordan modulo m (fabricando un pivote
//           cuando ninguno de la columna es invertible) y el calculo de la
//           inversa modular de una matriz.
// No hace: no define las operaciones basicas de matrices (ver matriz_basica.h).
// Lo usa: src/ataque/*, src/nucleo/hill.cpp, src/interfaz/main.cpp, src/pruebas/tests.cpp.
#ifndef GAUSS_JORDAN_H
#define GAUSS_JORDAN_H

#include "matriz.h"

// Gauss-Jordan módulo m sobre las primeras "columnasCoef" columnas de A.
// Deja A en forma escalonada reducida: cada pivote vale 1 y es el único
// número distinto de cero de su columna. Guarda en "columnasPivote" las
// columnas donde se encontró pivote y devuelve cuántos hubo (el rango).
int gaussJordan(Matriz& A, int columnasCoef, int m, std::vector<int>& columnasPivote);

// Inversa de K módulo m aumentando [K | I] y aplicando Gauss-Jordan.
// Devuelve false si K no es invertible módulo m.
bool inversaGaussJordan(const Matriz& K, int m, Matriz& inversa);

#endif
