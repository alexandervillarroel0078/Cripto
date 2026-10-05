#ifndef GAUSSJORDAN_H
#define GAUSSJORDAN_H

#include "Tipos.h"
#include <string>
#include <vector>

// Un paso del algoritmo de Gauss-Jordan: descripcion de la operacion
// y estado de la matriz aumentada [A | I] justo despues de aplicarla.
struct Paso {
    std::string descripcion;
    Mat         aumentada;   // n filas x 2n columnas
};

class GaussJordan {
public:
    // Invierte A mod 26. Devuelve matriz vacia si no es invertible.
    static Mat invertir(const Mat& A);
    // Igual que invertir, pero ademas registra cada operacion elemental en 'pasos'
    // (se vacia al inicio). Si A no es invertible, el ultimo paso explica el motivo.
    static Mat invertir(const Mat& A, std::vector<Paso>& pasos);

    // Recupera K = C * P^{-1} mod 26
    static Mat recuperarClave(const Mat& P, const Mat& C);
    // Igual, registrando los pasos de la inversion de P.
    static Mat recuperarClave(const Mat& P, const Mat& C, std::vector<Paso>& pasos);

private:
    static void intercambiarFilas(Mat& M, int i, int j);
    static void multiplicarFila(Mat& M, int i, int escalar);
    static void sumarFilas(Mat& M, int destino, int origen, int escalar);
};

#endif
