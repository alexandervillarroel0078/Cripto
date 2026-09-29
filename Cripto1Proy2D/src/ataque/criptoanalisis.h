// criptoanalisis.h
// Contiene: el punto de entrada del criptoanalisis de Hill (ataque de texto
//           claro conocido): arma el resultado combinando ataque directo,
//           sistema modular, TCR e inconsistencia.
// No hace: no implementa los metodos en si (ver ataque_directo.h,
//          sistema_modular.h, combinar_tcr.h, inconsistencia.h) ni las
//          correcciones de una letra (ver correcciones.h).
// Lo usa: main.cpp y tests.cpp.
//
// Si conocemos n bloques de texto claro y sus n bloques cifrados, los
// ponemos como COLUMNAS de P y C (n x n). Como cada bloque cumple c = K*p,
// todos juntos cumplen C = K*P, y si P es invertible mod 26:
//        K = C * P^-1  (mod 26)
#ifndef CRIPTOANALISIS_H
#define CRIPTOANALISIS_H

#include <string>
#include <vector>
#include "../nucleo/matriz.h"

struct ResultadoCriptoanalisis {
    bool exito;                 // true si se encontró al menos una clave válida
    std::string metodo;         // "directo" o "mod 13 / mod 2 + TCR"
    int detPrimerosBloques;     // det(P) mod 26 usando los primeros n bloques
    std::vector<Matriz> claves; // la clave (o todas las candidatas)

    // Diagnóstico cuando el par es inconsistente (no existe ninguna K).
    bool inconsistente;
    int moduloInconsistente;    // 13 o 2
    int rangoP;                 // rango de P en ese módulo
    int rangoPC;                // rango de la matriz aumentada [P|C]
    std::vector<int> columnasContradiccion; // columnas (1..n) con 0 = 1
};

// Recupera K (n x n) a partir de un texto claro y su criptograma: intenta
// primero el metodo directo y, si no alcanza, resuelve por modulos + TCR.
ResultadoCriptoanalisis criptoanalisis(const std::string& claro,
                                       const std::string& cripto, int n);

#endif
