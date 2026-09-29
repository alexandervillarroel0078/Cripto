// hill.h
// Cifrador de Hill sobre el alfabeto A-Z (A=0 ... Z=25), módulo 26.
// Convención: C = K * P, donde P es un bloque de n letras como vector columna.
#ifndef HILL_H
#define HILL_H

#include <string>
#include <vector>
#include "matriz.h"

const int MODULO = 26;

// Pasa a mayúsculas y elimina todo lo que no sea A-Z (espacios, números...).
std::string normalizar(const std::string& texto);

// "HELP" -> [7, 4, 11, 15]
std::vector<int> textoANumeros(const std::string& texto);

// [7, 4, 11, 15] -> "HELP"
std::string numerosATexto(const std::vector<int>& numeros);

// Multiplica cada bloque de n letras por la matriz M (mod 26).
// El texto ya debe estar normalizado y tener longitud múltiplo de n.
// Sirve tanto para cifrar (M = K) como para descifrar (M = K^-1).
std::string aplicarHill(const Matriz& M, const std::string& texto);

// Cifra: normaliza, rellena con "relleno" hasta un múltiplo de n y aplica K.
std::string cifrar(const Matriz& K, const std::string& texto, char relleno = 'X');

// Descifra: calcula K^-1 por Gauss-Jordan y la aplica.
// Devuelve "" si K no es invertible mod 26.
std::string descifrar(const Matriz& K, const std::string& cripto);

#endif
