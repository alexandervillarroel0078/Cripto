// correcciones.cpp
#include "correcciones.h"
#include "sistema_modular.h"
#include "combinar_tcr.h"
#include "../nucleo/hill.h"
#include "../nucleo/matriz_basica.h"
#include "../interfaz/traza.h"
#include <iostream>
#include <algorithm>

// En la busqueda de correcciones se prueban 16 x 25 variantes; ahi usamos
// un limite mas chico para que la busqueda termine rapido.
const long LIMITE_CORRECCION = 5000;

// Cambiar una letra del criptograma modifica un solo numero de C (una
// sola columna del sistema). Cambiar una letra del texto claro modifica
// P, es decir, los coeficientes de TODAS las ecuaciones.
std::vector<Correccion> buscarCorreccionUnaLetra(const std::string& claroOriginal, const std::string& criptoOriginal,
                                                 int n, bool modificarCripto) {
    std::vector<Correccion> lista;
    std::string claro = normalizar(claroOriginal);
    std::string cripto = normalizar(criptoOriginal);
    int bloques = (int)(std::min(claro.size(), cripto.size()) / n);
    if (bloques == 0) return lista;
    claro = claro.substr(0, bloques * n);
    cripto = cripto.substr(0, bloques * n);

    const std::string& texto = modificarCripto ? cripto : claro;

    for (int pos = 0; pos < (int)texto.size(); pos++) {
        for (char letra = 'A'; letra <= 'Z'; letra++) {
            if (letra == texto[pos]) continue;

            std::string c2 = cripto;
            std::string p2 = claro;
            if (modificarCripto) c2[pos] = letra;
            else p2[pos] = letra;

            std::vector<int> numClaro = textoANumeros(p2);
            std::vector<int> numCripto = textoANumeros(c2);

            InfoSistema i13 = resolverModPrimo(numClaro, numCripto, bloques, n, 13, LIMITE_CORRECCION, false);
            if (!i13.consistente) continue;
            InfoSistema i2 = resolverModPrimo(numClaro, numCripto, bloques, n, 2, LIMITE_CORRECCION, false);
            if (!i2.consistente) continue;

            Correccion c;
            c.posicion = pos + 1;
            c.original = texto[pos];
            c.nueva = letra;
            c.clavesEnumeradas = !i13.truncado && !i2.truncado &&
                                 (long)(i13.soluciones.size() * i2.soluciones.size()) <= LIMITE_CORRECCION;
            if (c.clavesEnumeradas) {
                c.claves = combinarTCR(i13.soluciones, i2.soluciones, n, p2, c2);
            }
            lista.push_back(c);
        }
    }
    return lista;
}

void imprimirCorrecciones(const std::vector<Correccion>& lista) {
    if (lista.empty()) {
        std::cout << "  Ninguna correccion de una letra hace consistente el sistema.\n";
        return;
    }
    std::cout << "  Correcciones que hacen consistente el sistema: " << lista.size() << "\n";
    bool trazaAnterior = MODO_TRAZA;
    MODO_TRAZA = false;
    for (size_t i = 0; i < lista.size(); i++) {
        const Correccion& c = lista[i];
        std::cout << "  " << (i + 1) << ") posicion " << c.posicion << ": "
                  << c.original << " -> " << c.nueva << "   claves validas: ";
        if (!c.clavesEnumeradas) {
            std::cout << "(demasiadas para enumerar)\n";
            continue;
        }
        std::cout << c.claves.size();
        if (!c.claves.empty()) {
            std::cout << "   det:";
            for (size_t k = 0; k < c.claves.size(); k++) {
                std::cout << " " << determinante(c.claves[k], MODULO);
            }
        }
        std::cout << "\n";
    }
    MODO_TRAZA = trazaAnterior;
}
