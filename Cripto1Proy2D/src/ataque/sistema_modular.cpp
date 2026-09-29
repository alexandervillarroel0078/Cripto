// sistema_modular.cpp
#include "sistema_modular.h"
#include "inconsistencia.h"
#include "../nucleo/modular.h"
#include "../nucleo/matriz_basica.h"
#include "../nucleo/gauss_jordan.h"
#include "../interfaz/traza.h"
#include <iostream>

// Arma la matriz aumentada [P^T | C^T] mod p (bloques filas, 2n columnas).
static Matriz armarSistemaModP(const std::vector<int>& numClaro, const std::vector<int>& numCripto,
                               int bloques, int n, int p) {
    Matriz A = crearMatriz(bloques, 2 * n);
    for (int b = 0; b < bloques; b++) {
        for (int j = 0; j < n; j++) {
            A[b][j] = mod(numClaro[b * n + j], p);
            A[b][n + j] = mod(numCripto[b * n + j], p);
        }
    }
    return A;
}

// Columnas sin pivote tras Gauss-Jordan = variables libres (0-indexadas).
static std::vector<int> columnasLibres(const std::vector<int>& pivotes, int n) {
    std::vector<int> libres;
    for (int c = 0; c < n; c++) {
        bool esPivote = false;
        for (size_t t = 0; t < pivotes.size(); t++) {
            if (pivotes[t] == c) esPivote = true;
        }
        if (!esPivote) libres.push_back(c);
    }
    return libres;
}

// Calcula p^(f*n) truncado en "limite": cantidad de soluciones a enumerar.
static long calcularTotal(int f, int n, int p, long limite, bool imprimir, bool& truncado) {
    int digitos = f * n;
    long total = 1;
    for (int d = 0; d < digitos && total <= limite; d++) {
        total *= p;
    }
    truncado = total > limite;
    if (!truncado) return total;
    if (imprimir) {
        std::cout << "  Aviso: demasiadas soluciones; se enumeran solo las primeras "
                  << limite << ".\n";
    }
    return limite;
}

// Enumera las asignaciones de las variables libres como un contador en base
// p (como un odometro) y arma cada K que cumple la forma reducida.
static std::vector<Matriz> enumerarSoluciones(const Matriz& A, const std::vector<int>& pivotes,
                                              const std::vector<int>& libres, int rango, int n, int p,
                                              long limite, bool imprimir, bool& truncado) {
    int f = (int)libres.size();
    long total = calcularTotal(f, n, p, limite, imprimir, truncado);
    int digitos = f * n;

    std::vector<Matriz> soluciones;
    std::vector<int> valor(digitos, 0);
    for (long s = 0; s < total; s++) {
        Matriz X = crearMatriz(n, n); // X = K^T (columna i de X = fila i de K)
        for (int i = 0; i < n; i++) {
            for (int k = 0; k < f; k++) {
                X[libres[k]][i] = valor[i * f + k];
            }
            // Cada fila de la forma reducida dice:
            //   x_pivote + suma(coef * x_libre) = lado derecho
            // asi que despejamos la variable pivote.
            for (int t = 0; t < rango; t++) {
                int v = A[t][n + i];
                for (int k = 0; k < f; k++) {
                    v -= A[t][libres[k]] * X[libres[k]][i];
                }
                X[pivotes[t]][i] = mod(v, p);
            }
        }
        soluciones.push_back(transpuesta(X));

        // Siguiente valor del odometro.
        for (int d = 0; d < digitos; d++) {
            valor[d]++;
            if (valor[d] < p) break;
            valor[d] = 0;
        }
    }
    return soluciones;
}

// Resuelve K*P = C mod un primo p usando TODOS los bloques conocidos.
// Trasponiendo: P^T * K^T = C^T. La matriz de coeficientes P^T es la misma
// para las n filas de K, asi que resolvemos todo con una sola matriz
// aumentada [P^T | C^T]: la columna j del lado derecho corresponde a la
// fila j de K (y a la letra j de cada bloque cifrado).
InfoSistema resolverModPrimo(const std::vector<int>& numClaro, const std::vector<int>& numCripto,
                             int bloques, int n, int p, long limite, bool imprimir) {
    InfoSistema info;
    info.consistente = true;
    info.truncado = false;
    info.libres = 0;

    bool trazaAnterior = MODO_TRAZA;
    if (!imprimir) MODO_TRAZA = false;

    Matriz A = armarSistemaModP(numClaro, numCripto, bloques, n, p);

    std::vector<int> pivotes;
    info.rango = gaussJordan(A, n, p, pivotes);

    // Rango de la aumentada: seguimos reduciendo tambien las columnas del
    // lado derecho (las operaciones de fila no cambian el rango).
    Matriz copia = A;
    std::vector<int> pivotesAumentada;
    MODO_TRAZA = false;
    info.rangoAumentada = gaussJordan(copia, 2 * n, p, pivotesAumentada);
    MODO_TRAZA = imprimir ? trazaAnterior : false;

    info.columnasContradiccion = detectarContradiccion(A, info.rango, info.rangoAumentada, bloques, n, p, imprimir);
    if (!info.columnasContradiccion.empty()) {
        info.consistente = false;
        MODO_TRAZA = trazaAnterior;
        return info;
    }

    std::vector<int> libres = columnasLibres(pivotes, n);
    info.libres = (int)libres.size();

    if (imprimir) {
        std::cout << "  mod " << p << ": rango(P) = " << info.rango << " = rango([P|C])"
                  << " -> consistente; variables libres por fila de K = " << info.libres
                  << "  ->  " << p << "^(" << info.libres << "*" << n << ") soluciones\n";
    }

    info.soluciones = enumerarSoluciones(A, pivotes, libres, info.rango, n, p, limite, imprimir, info.truncado);

    MODO_TRAZA = trazaAnterior;
    return info;
}
