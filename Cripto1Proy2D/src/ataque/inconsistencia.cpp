// inconsistencia.cpp
#include "inconsistencia.h"
#include <iostream>

// Teorema de Rouche-Frobenius: el sistema tiene solucion si y solo si
// rango(P) = rango([P|C]). Si el rango aumentado es mayor, alguna fila
// reducida queda [0 ... 0 | c] con c != 0, que dice "0 = c": imposible.
std::vector<int> detectarContradiccion(const Matriz& A, int rango, int rangoAumentada,
                                       int bloques, int n, int p, bool imprimir) {
    std::vector<int> columnasContradiccion;
    int filaContradiccion = -1;
    for (int j = 0; j < n; j++) {
        for (int r = rango; r < bloques; r++) {
            if (A[r][n + j] != 0) {
                columnasContradiccion.push_back(j + 1);
                if (filaContradiccion == -1) filaContradiccion = r;
                break;
            }
        }
    }
    if (columnasContradiccion.empty() || !imprimir) {
        return columnasContradiccion;
    }

    std::cout << "  Par inconsistente: rango(P mod " << p << ") = " << rango
              << ", rango([P|C] mod " << p << ") = " << rangoAumentada
              << " -> no existe K\n";
    std::cout << "  Fila " << (filaContradiccion + 1) << " reducida: [";
    for (int j = 0; j < 2 * n; j++) {
        if (j == n) std::cout << " |";
        std::cout << " " << A[filaContradiccion][j];
    }
    std::cout << " ]\n";
    std::cout << "  Contradiccion 0 = c (c != 0) en la(s) columna(s):";
    for (size_t t = 0; t < columnasContradiccion.size(); t++) {
        int col = columnasContradiccion[t];
        // Mostramos el valor de c de la primera fila que contradice.
        int c = 0;
        for (int r = rango; r < bloques && c == 0; r++) c = A[r][n + col - 1];
        std::cout << (t > 0 ? "," : "") << " " << col << " (0 = " << c << ")";
    }
    std::cout << "\n";
    std::cout << "  (columna j del sistema = fila j de K = letra j de cada bloque cifrado)\n";
    return columnasContradiccion;
}
