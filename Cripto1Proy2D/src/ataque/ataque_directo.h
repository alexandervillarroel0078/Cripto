// ataque_directo.h
// Contiene: metodo directo de criptoanalisis (buscar una combinacion de n
//           bloques con P invertible mod 26 y despejar K = C * P^-1).
// No hace: no resuelve por modulos primos ni maneja pares inconsistentes
//          (ver sistema_modular.h e inconsistencia.h).
// Lo usa: criptoanalisis.cpp, como primer metodo antes de intentar TCR.
#ifndef ATAQUE_DIRECTO_H
#define ATAQUE_DIRECTO_H

#include <string>
#include <vector>
#include "../nucleo/matriz.h"

// Resultado del intento del metodo directo.
struct ResultadoDirecto {
    bool exito;
    int detPrimerosBloques; // det(P) mod 26 de la primera combinacion probada (-1 si bloques < n)
    Matriz clave;            // valida solo si exito == true
};

// Prueba combinaciones de n bloques buscando una P invertible mod 26; si la
// encuentra, calcula K = C*P^-1 y verifica que cifre el texto conocido.
ResultadoDirecto intentarAtaqueDirecto(const std::vector<int>& numClaro, const std::vector<int>& numCripto,
                                       int bloques, int n, const std::string& claro, const std::string& cripto);

#endif
