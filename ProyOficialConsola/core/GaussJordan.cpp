#include "GaussJordan.h"
#include "MatrizMod26.h"
#include <algorithm>

void GaussJordan::intercambiarFilas(Mat& M, int i, int j) {
    std::swap(M[i], M[j]);
}

void GaussJordan::multiplicarFila(Mat& M, int i, int escalar) {
    for (size_t k = 0; k < M[i].size(); ++k)
        M[i][k] = MatrizMod26::mod(M[i][k] * escalar);
}

void GaussJordan::sumarFilas(Mat& M, int destino, int origen, int escalar) {
    for (size_t k = 0; k < M[destino].size(); ++k)
        M[destino][k] = MatrizMod26::mod(M[destino][k] + escalar * M[origen][k]);
}

// Version sin registro de pasos: delega en la sobrecarga completa.
Mat GaussJordan::invertir(const Mat& A) {
    std::vector<Paso> pasos;
    return invertir(A, pasos);
}

// Inversion por Gauss-Jordan sobre [A | I] en aritmetica mod 26.
// Cada operacion elemental se registra en 'pasos' con la matriz resultante.
Mat GaussJordan::invertir(const Mat& A, std::vector<Paso>& pasos) {
    pasos.clear();
    int n = (int)A.size();
    if (n == 0 || (int)A[0].size() != n) return Mat();

    // Construir [A | I]
    Mat M(n, Vec(2 * n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            M[i][j] = MatrizMod26::mod(A[i][j]);
        M[i][n + i] = 1;
    }
    pasos.push_back({"Matriz aumentada inicial [A | I]", M});

    for (int col = 0; col < n; ++col) {
        // Buscar pivote (desde la fila 'col' hacia abajo) coprimo con 26
        int piv = -1;
        for (int f = col; f < n; ++f) {
            if (MatrizMod26::inversoMod(M[f][col], MOD) != -1) {
                piv = f; break;
            }
        }
        if (piv == -1) {
            pasos.push_back({"Columna " + std::to_string(col + 1) +
                ": ningun elemento (desde la fila " + std::to_string(col + 1) +
                ") es coprimo con 26; A no es invertible mod 26", M});
            return Mat();
        }

        int inv = MatrizMod26::inversoMod(M[piv][col], MOD);
        pasos.push_back({"Columna " + std::to_string(col + 1) +
            ": pivote = " + std::to_string(M[piv][col]) + " (fila " +
            std::to_string(piv + 1) + "), coprimo con 26; inverso modular = " +
            std::to_string(inv) + " (" + std::to_string(M[piv][col]) + " * " +
            std::to_string(inv) + " = 1 mod 26)", M});

        // Intercambio de filas si el pivote no esta en la diagonal
        if (piv != col) {
            intercambiarFilas(M, col, piv);
            pasos.push_back({"F" + std::to_string(col + 1) + " <-> F" +
                std::to_string(piv + 1), M});
        }

        // Normalizar pivote a 1: Fi = inv * Fi mod 26
        if (inv != 1) {
            multiplicarFila(M, col, inv);
            pasos.push_back({"F" + std::to_string(col + 1) + " = " +
                std::to_string(inv) + " * F" + std::to_string(col + 1) +
                " mod 26", M});
        }

        // Anular el resto de la columna: Fj = Fj - c*Fi mod 26
        for (int f = 0; f < n; ++f) {
            if (f == col) continue;
            int factor = M[f][col];
            if (factor == 0) continue;
            sumarFilas(M, f, col, -factor);
            pasos.push_back({"F" + std::to_string(f + 1) + " = F" +
                std::to_string(f + 1) + " - " + std::to_string(factor) +
                " * F" + std::to_string(col + 1) + " mod 26", M});
        }
    }
    pasos.push_back({"Matriz final [I | A^-1]", M});

    // Extraer mitad derecha
    Mat Inv(n, Vec(n, 0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            Inv[i][j] = M[i][n + j];
    return Inv;
}

Mat GaussJordan::recuperarClave(const Mat& P, const Mat& C) {
    std::vector<Paso> pasos;
    return recuperarClave(P, C, pasos);
}

// K = C * P^-1 mod 26; los pasos corresponden a la inversion de P.
Mat GaussJordan::recuperarClave(const Mat& P, const Mat& C, std::vector<Paso>& pasos) {
    Mat Pinv = invertir(P, pasos);
    if (Pinv.empty()) return Mat();
    return MatrizMod26::multiplicar(C, Pinv);
}
