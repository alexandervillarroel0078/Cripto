// entrada.cpp
#include "entrada.h"
#include "../nucleo/matriz_basica.h"
#include "../nucleo/modular.h"
#include "../nucleo/hill.h"
#include <iostream>
#include <limits>

void limpiarEntrada() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int leerEntero(const std::string& mensaje) {
    int valor;
    while (true) {
        std::cout << mensaje;
        if (std::cin >> valor) {
            limpiarEntrada();
            return valor;
        }
        if (std::cin.eof()) {
            return 0; // fin de la entrada: tratamos como "Salir"
        }
        std::cout << "  Entrada invalida, ingrese un numero.\n";
        limpiarEntrada();
    }
}

std::string leerLinea(const std::string& mensaje) {
    std::cout << mensaje;
    std::string linea;
    std::getline(std::cin, linea);
    return linea;
}

int leerTamanio() {
    while (true) {
        int n = leerEntero("Tamanio del bloque n (2, 3 o 4): ");
        if (n >= 2 && n <= 4) return n;
        if (std::cin.eof()) return 2;
        std::cout << "  n debe ser 2, 3 o 4.\n";
    }
}

Matriz leerMatriz(int n) {
    Matriz K = crearMatriz(n, n);
    std::cout << "Ingrese la matriz K fila por fila (" << n << " numeros separados por espacio):\n";
    for (int i = 0; i < n; i++) {
        bool ok = false;
        while (!ok) {
            std::cout << "  Fila " << (i + 1) << ": ";
            ok = true;
            for (int j = 0; j < n && ok; j++) {
                if (!(std::cin >> K[i][j])) ok = false;
            }
            if (std::cin.eof()) return K;
            limpiarEntrada();
            if (!ok) std::cout << "  Fila invalida, vuelva a ingresarla.\n";
        }
        for (int j = 0; j < n; j++) {
            K[i][j] = mod(K[i][j], MODULO); // aceptamos negativos: -1 = 25
        }
    }
    return K;
}
