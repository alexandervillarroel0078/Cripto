// traza.h
// Impresión formateada de matrices, vectores y pasos intermedios.
// El MODO TRAZA es un interruptor global: cuando está activo, cada módulo
// imprime el detalle de sus cálculos (útil para explicar en la defensa).
#ifndef TRAZA_H
#define TRAZA_H

#include <string>
#include <vector>
#include "../nucleo/matriz.h"

// Interruptor global del modo traza (definido en traza.cpp).
extern bool MODO_TRAZA;

// Imprime una matriz con un título, alineando las columnas.
void imprimirMatriz(const Matriz& A, const std::string& titulo);

// Imprime una matriz aumentada [izq | der], con una barra después de
// la columna "columnasIzq".
void imprimirAumentada(const Matriz& A, int columnasIzq, const std::string& titulo);

// Imprime un vector entre corchetes: [7 4]
void imprimirVector(const std::vector<int>& v);

// Línea separadora para ordenar la salida.
void separador();

// Imprime un mensaje solo si el modo traza está activo.
void traza(const std::string& mensaje);

#endif
