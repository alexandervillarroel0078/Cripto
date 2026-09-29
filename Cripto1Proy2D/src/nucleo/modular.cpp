// modular.cpp
#include "modular.h"
#include "../interfaz/traza.h"
#include <iostream>
#include <iomanip>

int mod(int a, int m) {
    int r = a % m;
    // Si el resto es negativo le sumamos m: sigue siendo la misma clase
    // de equivalencia (a y a+m dejan el mismo resto) pero ahora es positivo.
    if (r < 0) {
        r = r + m;
    }
    return r;
}

int mcd(int a, int b) {
    a = (a < 0) ? -a : a;
    b = (b < 0) ? -b : b;
    // Euclides: mcd(a, b) = mcd(b, a mod b), porque todo divisor común de
    // a y b también divide a (a - q*b), que es el resto.
    while (b != 0) {
        int r = a % b;
        a = b;
        b = r;
    }
    return a;
}

int euclidesExtendido(int a, int b, int& x, int& y) {
    // Versión iterativa. Mantenemos dos filas (r, s, t) que cumplen siempre
    //     r = a*s + b*t
    // Al principio: a = a*1 + b*0   y   b = a*0 + b*1.
    // En cada paso restamos q veces la fila nueva a la vieja (igual que en
    // Euclides con los restos), así el invariante se conserva. Cuando el
    // resto llega a 0, la fila anterior tiene r = mcd y sus s, t son x, y.
    int r0 = a, r1 = b;
    int s0 = 1, s1 = 0;
    int t0 = 0, t1 = 1;

    if (MODO_TRAZA) {
        std::cout << "  Euclides extendido para (" << a << ", " << b << "):\n";
    }

    while (r1 != 0) {
        int q = r0 / r1;
        int r2 = r0 - q * r1;
        int s2 = s0 - q * s1;
        int t2 = t0 - q * t1;

        if (MODO_TRAZA) {
            std::cout << "    " << std::setw(4) << r0 << " = " << std::setw(3) << q
                      << " * " << std::setw(3) << r1 << " + " << std::setw(3) << r2
                      << "      (s = " << s2 << ", t = " << t2 << ")\n";
        }

        r0 = r1; r1 = r2;
        s0 = s1; s1 = s2;
        t0 = t1; t1 = t2;
    }

    x = s0;
    y = t0;
    if (MODO_TRAZA) {
        std::cout << "    mcd = " << r0 << "  y  " << a << "*(" << x << ") + "
                  << b << "*(" << y << ") = " << r0 << "\n";
    }
    return r0;
}

int inversoModular(int a, int m) {
    a = mod(a, m);
    int x, y;
    // Si a*x + m*y = 1, al tomar mod m el término m*y desaparece y queda
    // a*x = 1 (mod m): x es el inverso. Esto solo es posible si mcd = 1.
    int d = euclidesExtendido(a, m, x, y);
    if (d != 1) {
        if (MODO_TRAZA) {
            std::cout << "    mcd(" << a << ", " << m << ") = " << d
                      << " != 1  ->  " << a << " NO tiene inverso mod " << m << "\n";
        }
        return -1;
    }
    int inv = mod(x, m);
    if (MODO_TRAZA) {
        std::cout << "    inverso de " << a << " mod " << m << " = " << x
                  << " mod " << m << " = " << inv << "   (comprobacion: "
                  << a << "*" << inv << " = " << a * inv << " = 1 mod " << m << ")\n";
    }
    return inv;
}
