// combinar_tcr.cpp
#include "combinar_tcr.h"
#include "../nucleo/hill.h"
#include "../nucleo/modular.h"
#include "../nucleo/matriz_basica.h"
#include "../interfaz/traza.h"

// Verifica que K cifre el texto claro conocido en el criptograma conocido.
static bool verificarClave(const Matriz& K, const std::string& claro, const std::string& cripto) {
    bool trazaAnterior = MODO_TRAZA;
    MODO_TRAZA = false;
    bool ok = (aplicarHill(K, claro) == cripto);
    MODO_TRAZA = trazaAnterior;
    return ok;
}

// Buscamos e13 y e2 tales que
//   e13 = 1 (mod 13), e13 = 0 (mod 2)   y   e2 = 0 (mod 13), e2 = 1 (mod 2)
// Entonces x = a*e13 + b*e2 cumple x = a (mod 13) y x = b (mod 2).
// e13 = 2 * inv(2 mod 13) = 14   y   e2 = 13 * inv(13 mod 2) = 13.
std::vector<Matriz> combinarTCR(const std::vector<Matriz>& sol13, const std::vector<Matriz>& sol2,
                                int n, const std::string& claro, const std::string& cripto) {
    bool trazaAnterior = MODO_TRAZA;
    MODO_TRAZA = false;
    int e13 = mod(2 * inversoModular(2, 13), MODULO);
    int e2 = mod(13 * inversoModular(13, 2), MODULO);

    std::vector<Matriz> claves;
    for (size_t a = 0; a < sol13.size(); a++) {
        for (size_t b = 0; b < sol2.size(); b++) {
            Matriz K = crearMatriz(n, n);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    K[i][j] = mod(e13 * sol13[a][i][j] + e2 * sol2[b][i][j], MODULO);
                }
            }
            int det = determinante(K, MODULO);
            if (mcd(det, MODULO) == 1 && verificarClave(K, claro, cripto)) {
                claves.push_back(K);
            }
        }
    }
    MODO_TRAZA = trazaAnterior;
    return claves;
}
