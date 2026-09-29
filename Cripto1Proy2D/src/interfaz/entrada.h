// entrada.h
// Contiene: lectura de datos por consola (enteros, líneas, tamaño de bloque
//           y matrices) con reintento ante entrada inválida.
// No hace: no implementa las opciones del menú ni el bucle principal (ver menu.cpp).
// Lo usa: menu.cpp.
#ifndef ENTRADA_H
#define ENTRADA_H

#include <string>
#include "../nucleo/matriz.h"

// Descarta lo que quede en la línea actual de entrada (por ejemplo el
// Enter que deja "cin >>"), para que el próximo getline lea bien.
void limpiarEntrada();

// Lee un entero; si el usuario escribe algo que no es número, vuelve a pedir.
int leerEntero(const std::string& mensaje);

// Lee una línea completa de la entrada estándar.
std::string leerLinea(const std::string& mensaje);

// Lee el tamaño de bloque n (2, 3 o 4), reintentando si es inválido.
int leerTamanio();

// Lee la matriz K fila por fila: n números separados por espacios.
Matriz leerMatriz(int n);

#endif
