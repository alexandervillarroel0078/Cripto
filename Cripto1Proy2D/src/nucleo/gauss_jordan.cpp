// gauss_jordan.cpp
#include "gauss_jordan.h"
#include "matriz_basica.h"
#include "modular.h"
#include "../interfaz/traza.h"
#include <iostream>
#include <sstream>
#include <utility>

// Imprime el nombre de una operación de fila y la matriz resultante.
static void mostrarPaso(const std::string& operacion, const Matriz& A, int columnasCoef) {
    if (MODO_TRAZA) {
        std::cout << "  " << operacion << "\n";
        imprimirAumentada(A, columnasCoef, "");
    }
}

int gaussJordan(Matriz& A, int columnasCoef, int m, std::vector<int>& columnasPivote) {
    int filas = (int)A.size();
    int columnasTotales = (int)A[0].size();
    columnasPivote.clear();

    if (MODO_TRAZA) {
        std::cout << "  Gauss-Jordan mod " << m << ":\n";
        imprimirAumentada(A, columnasCoef, "  Matriz inicial:");
    }

    int fila = 0; // fila donde va el próximo pivote
    for (int col = 0; col < columnasCoef && fila < filas; col++) {

        // 1) Buscar un pivote INVERTIBLE mod m (mcd(pivote, m) = 1).
        //    En mod 26 no basta con que sea distinto de 0: por ejemplo 2 o 13
        //    no tienen inverso, y sin inverso no podemos convertirlo en 1.
        int piv = -1;
        for (int r = fila; r < filas; r++) {
            if (mcd(A[r][col], m) == 1) {
                piv = r;
                break;
            }
        }

        // 2) Si ninguno es invertible, intentamos "fabricar" uno sumando otra
        //    fila (operación elemental válida). Esto SIEMPRE encuentra pivote
        //    cuando m = 26 y la matriz es invertible: si ningún elemento de la
        //    columna es invertible mod 26, cada uno es par o múltiplo de 13
        //    (los únicos no invertibles, porque 26 = 2*13). Como la matriz es
        //    invertible mod 2, esta columna no puede ser toda par -> hay un
        //    múltiplo de 13 que además es impar, o sea el único no invertible
        //    posible: 13. Como la matriz también es invertible mod 13, la
        //    columna no puede ser toda múltiplo de 13 -> hay un par distinto
        //    de 0. La suma de ese 13 con ese par es impar (13 + par) y no es
        //    múltiplo de 13 (13 + par = 13 solo si par = 0, que ya excluimos):
        //    por lo tanto esa suma sí es invertible mod 26. Ej.: 2 y 13 no son
        //    invertibles, pero 2 + 13 = 15 sí lo es.
        if (piv == -1) {
            for (int r = fila; r < filas && piv == -1; r++) {
                for (int r2 = fila; r2 < filas && piv == -1; r2++) {
                    if (r2 == r) continue;
                    if (mcd(A[r][col] + A[r2][col], m) == 1) {
                        for (int j = 0; j < columnasTotales; j++) {
                            A[r][j] = mod(A[r][j] + A[r2][j], m);
                        }
                        piv = r;
                        std::ostringstream op;
                        op << "F" << (r + 1) << " <- F" << (r + 1) << " + F" << (r2 + 1)
                           << " (mod " << m << ")   [ningun pivote era invertible]";
                        mostrarPaso(op.str(), A, columnasCoef);
                    }
                }
            }
        }

        if (piv == -1) {
            // La columna no tiene pivote: corresponde a una variable libre.
            if (MODO_TRAZA) {
                std::cout << "  Columna " << (col + 1)
                          << ": no hay pivote invertible -> variable libre\n";
            }
            continue;
        }

        // 3) Intercambio de filas para llevar el pivote a su lugar.
        if (piv != fila) {
            std::swap(A[piv], A[fila]);
            std::ostringstream op;
            op << "F" << (fila + 1) << " <-> F" << (piv + 1)
               << "   [el pivote de la fila " << (fila + 1) << " no era invertible]";
            mostrarPaso(op.str(), A, columnasCoef);
        }

        // 4) Normalizar: multiplicar la fila por el inverso del pivote para
        //    que el pivote valga 1 (a * a^-1 = 1 mod m).
        //    Apagamos la traza de Euclides aquí para no repetirla en cada pivote.
        bool trazaAnterior = MODO_TRAZA;
        MODO_TRAZA = false;
        int inv = inversoModular(A[fila][col], m);
        MODO_TRAZA = trazaAnterior;

        if (inv != 1) {
            int pivote = A[fila][col];
            for (int j = 0; j < columnasTotales; j++) {
                A[fila][j] = mod(A[fila][j] * inv, m);
            }
            std::ostringstream op;
            op << "F" << (fila + 1) << " <- " << inv << "*F" << (fila + 1)
               << " (mod " << m << ")   [" << inv << " es el inverso de " << pivote
               << ": " << pivote << "*" << inv << " = 1 mod " << m << "]";
            mostrarPaso(op.str(), A, columnasCoef);
        }

        // 5) Eliminar: hacer 0 el resto de la columna. Si la fila r tiene el
        //    valor f en esta columna, F_r - f*F_pivote deja ahí f - f*1 = 0.
        for (int r = 0; r < filas; r++) {
            if (r == fila) continue;
            int factor = A[r][col];
            if (factor == 0) continue;
            for (int j = 0; j < columnasTotales; j++) {
                A[r][j] = mod(A[r][j] - factor * A[fila][j], m);
            }
            std::ostringstream op;
            op << "F" << (r + 1) << " <- F" << (r + 1) << " - " << factor
               << "*F" << (fila + 1) << " (mod " << m << ")";
            mostrarPaso(op.str(), A, columnasCoef);
        }

        columnasPivote.push_back(col);
        fila++;
    }
    return fila;
}

bool inversaGaussJordan(const Matriz& K, int m, Matriz& inversa) {
    int n = (int)K.size();

    // Armamos [K | I]. Las operaciones de fila que convierten K en I son
    // equivalentes a multiplicar por K^-1 a la izquierda; aplicadas también
    // a I, la convierten en K^-1 * I = K^-1.
    Matriz A = crearMatriz(n, 2 * n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = mod(K[i][j], m);
        }
        A[i][n + i] = 1;
    }

    std::vector<int> pivotes;
    int rango = gaussJordan(A, n, m, pivotes);

    // Si no hubo n pivotes, la parte izquierda no llegó a ser la identidad.
    if (rango < n) {
        if (MODO_TRAZA) {
            std::cout << "  Solo se encontraron " << rango << " pivotes de " << n
                      << ": la matriz NO es invertible mod " << m << "\n";
        }
        return false;
    }

    inversa = crearMatriz(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inversa[i][j] = A[i][n + j];
        }
    }
    if (MODO_TRAZA) {
        std::cout << "  La parte izquierda es la identidad; la derecha es la inversa.\n";
    }
    return true;
}
