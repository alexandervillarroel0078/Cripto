// matriz.h
// Contiene: el tipo Matriz (vector de filas de enteros), sin operaciones.
// No hace: no declara construir, operar ni resolver matrices (ver
//          matriz_basica.h y gauss_jordan.h).
// Lo usa: todo el proyecto, para pasar matrices entre modulos.
#ifndef MATRIZ_H
#define MATRIZ_H

#include <vector>

// Una matriz es simplemente un vector de filas; cada fila es un vector de int.
typedef std::vector<std::vector<int> > Matriz;

#endif
