// inconsistencia.h
// Contiene: deteccion de sistemas sin solucion mod p via Rouche-Frobenius
//           (rango(P) vs rango([P|C])) y el mensaje que explica la contradiccion.
// No hace: no resuelve el sistema (ver sistema_modular.h) ni decide que
//          hacer con el resultado a nivel de K mod 26 (criptoanalisis.cpp).
// Lo usa: sistema_modular.cpp, al terminar Gauss-Jordan mod p.
#ifndef INCONSISTENCIA_H
#define INCONSISTENCIA_H

#include <vector>
#include "../nucleo/matriz.h"

// Busca filas reducidas [0 ... 0 | c] con c != 0 (columnas 1..n del sistema
// que quedan como "0 = c"). Devuelve esas columnas (vacio si es consistente).
// Si imprimir es true, muestra el mensaje de Rouche-Frobenius con el detalle.
std::vector<int> detectarContradiccion(const Matriz& A, int rango, int rangoAumentada,
                                       int bloques, int n, int p, bool imprimir);

#endif
