// matriz_basica.cpp
#include "matriz_basica.h"
#include "modular.h"
#include "../interfaz/traza.h"
#include <iostream>
#include <sstream>

Matriz crearMatriz(int filas, int columnas) {
    return Matriz(filas, std::vector<int>(columnas, 0));
}

Matriz identidad(int n) {
    Matriz I = crearMatriz(n, n);
    for (int i = 0; i < n; i++) {
        I[i][i] = 1;
    }
    return I;
}

Matriz transpuesta(const Matriz& A) {
    int filas = (int)A.size();
    int columnas = (int)A[0].size();
    Matriz T = crearMatriz(columnas, filas);
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            T[j][i] = A[i][j];
        }
    }
    return T;
}

Matriz multiplicar(const Matriz& A, const Matriz& B, int m) {
    int f = (int)A.size();
    int k = (int)B.size();
    int c = (int)B[0].size();
    Matriz R = crearMatriz(f, c);
    // (A*B)[i][j] = suma de A[i][t] * B[t][j]. Reducimos mod m al final de
    // cada suma: como el mod respeta sumas y productos, el resultado es el
    // mismo que reducir en cada paso (con n <= 4 no hay riesgo de overflow).
    for (int i = 0; i < f; i++) {
        for (int j = 0; j < c; j++) {
            int suma = 0;
            for (int t = 0; t < k; t++) {
                suma += A[i][t] * B[t][j];
            }
            R[i][j] = mod(suma, m);
        }
    }
    return R;
}

// Matriz que resulta de quitar la fila "fila" y la columna "col" (el menor).
static Matriz menor(const Matriz& A, int fila, int col) {
    int n = (int)A.size();
    Matriz M;
    for (int i = 0; i < n; i++) {
        if (i == fila) continue;
        std::vector<int> nuevaFila;
        for (int j = 0; j < n; j++) {
            if (j == col) continue;
            nuevaFila.push_back(A[i][j]);
        }
        M.push_back(nuevaFila);
    }
    return M;
}

// Determinante recursivo sin traza (se usa para los menores).
static int determinanteRec(const Matriz& A, int m) {
    int n = (int)A.size();
    if (n == 1) {
        return mod(A[0][0], m);
    }
    if (n == 2) {
        // Caso base 2x2: ad - bc
        return mod(A[0][0] * A[1][1] - A[0][1] * A[1][0], m);
    }
    // Expansión por cofactores en la primera fila:
    // det(A) = suma_j (-1)^j * a[0][j] * det(menor(0, j))
    int det = 0;
    for (int j = 0; j < n; j++) {
        int signo = (j % 2 == 0) ? 1 : -1;
        int d = determinanteRec(menor(A, 0, j), m);
        det = mod(det + signo * A[0][j] * d, m);
    }
    return det;
}

int determinante(const Matriz& A, int m) {
    int n = (int)A.size();
    int det = determinanteRec(A, m);

    if (MODO_TRAZA) {
        std::cout << "  Determinante mod " << m << " (expansion por la primera fila):\n";
        if (n == 1) {
            std::cout << "    det = " << A[0][0] << " = " << det << " (mod " << m << ")\n";
        } else if (n == 2) {
            int bruto = A[0][0] * A[1][1] - A[0][1] * A[1][0];
            std::cout << "    det = " << A[0][0] << "*" << A[1][1] << " - "
                      << A[0][1] << "*" << A[1][0] << " = " << bruto
                      << " = " << det << " (mod " << m << ")\n";
        } else {
            // Mostramos cada término a[0][j] * (-1)^j * det(menor)
            std::ostringstream suma;
            int bruto = 0;
            for (int j = 0; j < n; j++) {
                int signo = (j % 2 == 0) ? 1 : -1;
                int d = determinanteRec(menor(A, 0, j), m);
                std::cout << "    termino " << (j + 1) << ": "
                          << (signo > 0 ? "+" : "-") << " " << A[0][j]
                          << " * det(menor 1," << (j + 1) << ") = "
                          << (signo > 0 ? "+" : "-") << " " << A[0][j]
                          << " * " << d << " = " << signo * A[0][j] * d << "\n";
                bruto += signo * A[0][j] * d;
            }
            std::cout << "    det = " << bruto << " = " << det << " (mod " << m << ")\n";
        }
    }
    return det;
}

bool matricesIguales(const Matriz& A, const Matriz& B) {
    return A == B;
}
