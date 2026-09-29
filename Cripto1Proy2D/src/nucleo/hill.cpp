// hill.cpp
#include "hill.h"
#include "modular.h"
#include "gauss_jordan.h"
#include "../interfaz/traza.h"
#include <iostream>

std::string normalizar(const std::string& texto) {
    std::string r;
    for (size_t i = 0; i < texto.size(); i++) {
        char c = texto[i];
        if (c >= 'a' && c <= 'z') {
            c = (char)(c - 'a' + 'A');
        }
        if (c >= 'A' && c <= 'Z') {
            r += c;
        }
    }
    return r;
}

std::vector<int> textoANumeros(const std::string& texto) {
    std::vector<int> v;
    for (size_t i = 0; i < texto.size(); i++) {
        v.push_back(texto[i] - 'A'); // 'A' -> 0, 'B' -> 1, ... 'Z' -> 25
    }
    return v;
}

std::string numerosATexto(const std::vector<int>& numeros) {
    std::string s;
    for (size_t i = 0; i < numeros.size(); i++) {
        s += (char)('A' + mod(numeros[i], MODULO));
    }
    return s;
}

std::string aplicarHill(const Matriz& M, const std::string& texto) {
    int n = (int)M.size();
    std::vector<int> numeros = textoANumeros(texto);
    std::vector<int> salida;

    for (size_t inicio = 0; inicio + n <= numeros.size(); inicio += n) {
        // Bloque P (vector columna de n números)
        std::vector<int> P(numeros.begin() + inicio, numeros.begin() + inicio + n);

        if (MODO_TRAZA) {
            std::cout << "  Bloque " << (inicio / n + 1) << ": \""
                      << texto.substr(inicio, n) << "\" -> ";
            imprimirVector(P);
            std::cout << "\n";
        }

        // C = M * P : cada componente es el producto de una fila de M por P.
        for (int i = 0; i < n; i++) {
            int suma = 0;
            for (int j = 0; j < n; j++) {
                suma += M[i][j] * P[j];
            }
            int c = mod(suma, MODULO);
            salida.push_back(c);

            if (MODO_TRAZA) {
                // r_i = fila i de M por el bloque (c_i al cifrar, p_i al descifrar)
                std::cout << "    r" << (i + 1) << " = ";
                for (int j = 0; j < n; j++) {
                    if (j > 0) std::cout << " + ";
                    std::cout << M[i][j] << "*" << P[j];
                }
                std::cout << " = " << suma << " = " << c << " (mod 26) -> "
                          << (char)('A' + c) << "\n";
            }
        }
    }
    return numerosATexto(salida);
}

std::string cifrar(const Matriz& K, const std::string& texto, char relleno) {
    int n = (int)K.size();
    std::string t = normalizar(texto);

    // Si la longitud no es múltiplo de n, el último bloque quedaría
    // incompleto: lo completamos con la letra de relleno.
    while (t.size() % n != 0) {
        t += relleno;
    }

    if (MODO_TRAZA) {
        std::cout << "  Texto normalizado (con relleno): " << t << "\n";
    }
    return aplicarHill(K, t);
}

std::string descifrar(const Matriz& K, const std::string& cripto) {
    int n = (int)K.size();
    std::string t = normalizar(cripto);

    if (t.size() % n != 0) {
        std::cout << "  Aviso: el criptograma no es multiplo de " << n
                  << "; se completa con 'X'.\n";
        while (t.size() % n != 0) {
            t += 'X';
        }
    }

    // Como C = K*P, multiplicando por K^-1 a la izquierda: K^-1*C = P.
    Matriz Kinv;
    if (!inversaGaussJordan(K, MODULO, Kinv)) {
        return "";
    }
    if (MODO_TRAZA) {
        imprimirMatriz(Kinv, "  K^-1 (mod 26):");
    }
    return aplicarHill(Kinv, t);
}
