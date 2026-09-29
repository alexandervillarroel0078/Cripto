// ataque_directo.cpp
#include "ataque_directo.h"
#include "../nucleo/hill.h"
#include "../nucleo/modular.h"
#include "../nucleo/matriz_basica.h"
#include "../nucleo/gauss_jordan.h"
#include "../interfaz/traza.h"
#include <iostream>

// Arma P y C (n x n) poniendo como columnas los bloques indicados.
static void armarMatrices(const std::vector<int>& numClaro, const std::vector<int>& numCripto,
                          const std::vector<int>& bloques, int n, Matriz& P, Matriz& C) {
    P = crearMatriz(n, n);
    C = crearMatriz(n, n);
    for (int k = 0; k < n; k++) {
        int b = bloques[k];
        for (int i = 0; i < n; i++) {
            P[i][k] = numClaro[b * n + i];
            C[i][k] = numCripto[b * n + i];
        }
    }
}

// Avanza a la siguiente combinacion de "comb.size()" bloques elegidos entre
// "total" (orden lexicografico: {0,1}, {0,2}, ..., {1,2}, ...).
static bool siguienteCombinacion(std::vector<int>& comb, int total) {
    int k = (int)comb.size();
    int i = k - 1;
    while (i >= 0 && comb[i] == total - k + i) {
        i--;
    }
    if (i < 0) return false;
    comb[i]++;
    for (int j = i + 1; j < k; j++) {
        comb[j] = comb[j - 1] + 1;
    }
    return true;
}

static void imprimirBloques(const std::vector<int>& bloques) {
    std::cout << "{";
    for (size_t i = 0; i < bloques.size(); i++) {
        if (i > 0) std::cout << ",";
        std::cout << (bloques[i] + 1);
    }
    std::cout << "}";
}

// Resultado de probar una combinacion de bloques.
struct IntentoCombinacion {
    int det;
    bool exito;
    Matriz clave;
};

// Prueba una combinacion: si P es invertible, calcula K = C*P^-1 y verifica
// que cifre el texto conocido. Imprime el mismo detalle que el metodo original.
static IntentoCombinacion probarCombinacion(const std::vector<int>& numClaro, const std::vector<int>& numCripto,
                                            const std::vector<int>& comb, int n,
                                            const std::string& claro, const std::string& cripto) {
    IntentoCombinacion r;
    r.exito = false;

    Matriz P, C;
    armarMatrices(numClaro, numCripto, comb, n, P, C);
    if (MODO_TRAZA) {
        std::cout << "\n  Probando bloques ";
        imprimirBloques(comb);
        std::cout << "\n";
        imprimirMatriz(P, "  P (bloques de texto claro como columnas):");
        imprimirMatriz(C, "  C (bloques cifrados como columnas):");
    }
    r.det = determinante(P, MODULO);
    int g = mcd(r.det, MODULO);
    std::cout << "  Bloques ";
    imprimirBloques(comb);
    std::cout << ": det(P) = " << r.det << " (mod 26), mcd(" << r.det << ",26) = " << g;

    if (g != 1) {
        std::cout << "  -> P no invertible, se prueba otra combinacion\n";
        return r;
    }
    std::cout << "  -> P invertible\n";

    Matriz Pinv;
    inversaGaussJordan(P, MODULO, Pinv);
    Matriz K = multiplicar(C, Pinv, MODULO);
    if (MODO_TRAZA) {
        imprimirMatriz(Pinv, "  P^-1 (mod 26):");
        imprimirMatriz(K, "  K = C * P^-1 (mod 26):");
    }

    std::string verif = aplicarHill(K, claro);
    std::cout << "  Verificacion: cifrar(claro) con K = " << verif
              << (verif == cripto ? "  == criptograma  -> OK\n" : "  != criptograma  -> FALLA\n");
    if (verif == cripto) {
        r.exito = true;
        r.clave = K;
    }
    return r;
}

ResultadoDirecto intentarAtaqueDirecto(const std::vector<int>& numClaro, const std::vector<int>& numCripto,
                                       int bloques, int n, const std::string& claro, const std::string& cripto) {
    ResultadoDirecto res;
    res.exito = false;
    res.detPrimerosBloques = -1;

    if (bloques < n) {
        std::cout << "  Hay menos de " << n << " bloques: no se puede formar P de " << n
                  << "x" << n << ". Se resuelve el sistema por modulos.\n";
        return res;
    }

    std::vector<int> comb;
    for (int i = 0; i < n; i++) comb.push_back(i);

    bool primera = true;
    do {
        IntentoCombinacion intento = probarCombinacion(numClaro, numCripto, comb, n, claro, cripto);
        if (primera) {
            res.detPrimerosBloques = intento.det;
            primera = false;
        }
        if (intento.exito) {
            res.exito = true;
            res.clave = intento.clave;
            return res;
        }
    } while (siguienteCombinacion(comb, bloques));

    std::cout << "  Ninguna combinacion de " << n << " bloques da una P invertible mod 26.\n";
    return res;
}
