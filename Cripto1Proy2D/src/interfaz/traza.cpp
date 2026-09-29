// traza.cpp
#include "traza.h"
#include <iostream>
#include <iomanip>

bool MODO_TRAZA = false;

void imprimirMatriz(const Matriz& A, const std::string& titulo) {
    if (!titulo.empty()) {
        std::cout << titulo << "\n";
    }
    for (size_t i = 0; i < A.size(); i++) {
        std::cout << "    [";
        for (size_t j = 0; j < A[i].size(); j++) {
            std::cout << std::setw(4) << A[i][j];
        }
        std::cout << " ]\n";
    }
}

void imprimirAumentada(const Matriz& A, int columnasIzq, const std::string& titulo) {
    if (!titulo.empty()) {
        std::cout << titulo << "\n";
    }
    for (size_t i = 0; i < A.size(); i++) {
        std::cout << "    [";
        for (size_t j = 0; j < A[i].size(); j++) {
            if ((int)j == columnasIzq) {
                std::cout << "  |";
            }
            std::cout << std::setw(4) << A[i][j];
        }
        std::cout << " ]\n";
    }
}

void imprimirVector(const std::vector<int>& v) {
    std::cout << "[";
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) std::cout << " ";
        std::cout << v[i];
    }
    std::cout << "]";
}

void separador() {
    std::cout << "------------------------------------------------------------\n";
}

void traza(const std::string& mensaje) {
    if (MODO_TRAZA) {
        std::cout << mensaje << "\n";
    }
}
