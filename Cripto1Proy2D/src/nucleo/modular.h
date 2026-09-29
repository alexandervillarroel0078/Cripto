// modular.h
// Aritmética modular básica, implementada desde cero.
#ifndef MODULAR_H
#define MODULAR_H

// Residuo SIEMPRE en el rango [0, m-1].
// En C++ el operador % puede devolver negativos (-3 % 26 == -3),
// pero en aritmética modular -3 y 23 son la misma clase módulo 26.
int mod(int a, int m);

// Máximo común divisor por el algoritmo de Euclides.
int mcd(int a, int b);

// Euclides extendido: devuelve d = mcd(a,b) y encuentra x, y tales que
// a*x + b*y = d   (identidad de Bézout).
int euclidesExtendido(int a, int b, int& x, int& y);

// Inverso de a módulo m: el número x con a*x = 1 (mod m).
// Existe solo si mcd(a, m) = 1. Si no existe devuelve -1.
int inversoModular(int a, int m);

#endif
