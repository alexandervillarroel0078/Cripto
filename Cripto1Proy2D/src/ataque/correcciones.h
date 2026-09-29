// correcciones.h
// Contiene: busqueda de correcciones de UNA letra (texto claro o cifrado)
//           que vuelven consistente el sistema K*P = C, y su impresion.
// No hace: no decide cuando ofrecer la busqueda (eso es main.cpp) ni
//          resuelve el sistema completo (sistema_modular.h / combinar_tcr.h).
// Lo usa: main.cpp y tests.cpp.
#ifndef CORRECCIONES_H
#define CORRECCIONES_H

#include <string>
#include <vector>
#include "../nucleo/matriz.h"

// Una correccion de una letra que vuelve consistente el sistema K*P = C.
struct Correccion {
    int posicion;               // posicion de la letra (1 = primera)
    char original;
    char nueva;
    bool clavesEnumeradas;      // false si habia demasiadas soluciones para listarlas
    std::vector<Matriz> claves; // claves validas (det coprimo con 26) con la correccion
};

// Prueba cambiar UNA letra (cada posicion x las otras 25 letras) del
// criptograma (modificarCripto = true) o del texto claro (false) e informa
// que cambios hacen que el sistema tenga solucion mod 13 y mod 2.
std::vector<Correccion> buscarCorreccionUnaLetra(const std::string& claro, const std::string& cripto,
                                                 int n, bool modificarCripto);

// Imprime la lista de correcciones en forma de tabla.
void imprimirCorrecciones(const std::vector<Correccion>& lista);

#endif
