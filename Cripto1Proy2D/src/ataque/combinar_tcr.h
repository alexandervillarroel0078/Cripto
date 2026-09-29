// combinar_tcr.h
// Contiene: combinacion de soluciones mod 13 y mod 2 con el Teorema Chino
//           del Resto para obtener candidatas a K mod 26, filtrando las que
//           tienen det coprimo con 26 y cifran bien el texto conocido.
// No hace: no resuelve los sistemas mod 13 / mod 2 (ver sistema_modular.h).
// Lo usa: criptoanalisis.cpp y correcciones.cpp.
#ifndef COMBINAR_TCR_H
#define COMBINAR_TCR_H

#include <string>
#include <vector>
#include "../nucleo/matriz.h"

// Combina cada solucion mod 13 con cada solucion mod 2 (TCR) y devuelve las
// K mod 26 con det coprimo con 26 que cifran correctamente el texto conocido.
std::vector<Matriz> combinarTCR(const std::vector<Matriz>& sol13, const std::vector<Matriz>& sol2,
                                int n, const std::string& claro, const std::string& cripto);

#endif
