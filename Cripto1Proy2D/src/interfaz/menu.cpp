// menu.cpp
// ELC107 Criptografía y Seguridad
// Cifrador de Hill y Criptoanálisis por Gauss-Jordan (aplicación de consola).
#include <iostream>
#include <string>
#include <algorithm>
#include "../nucleo/matriz.h"
#include "../nucleo/matriz_basica.h"
#include "../nucleo/gauss_jordan.h"
#include "../nucleo/modular.h"
#include "../nucleo/hill.h"
#include "../ataque/criptoanalisis.h"
#include "../ataque/correcciones.h"
#include "traza.h"
#include "entrada.h"
#include "../pruebas/tests.h"

static void opcionCifrar() {
    int n = leerTamanio();
    Matriz K = leerMatriz(n);
    std::string texto = leerLinea("Texto claro: ");
    std::string r = leerLinea("Letra de relleno [Enter = X]: ");
    std::string rn = normalizar(r);
    char relleno = rn.empty() ? 'X' : rn[0];

    int det = determinante(K, MODULO);
    if (mcd(det, MODULO) != 1) {
        std::cout << "Aviso: det(K) = " << det << " no es coprimo con 26; se podra cifrar "
                  << "pero NO descifrar.\n";
    }
    std::string c = cifrar(K, texto, relleno);
    std::cout << "Criptograma: " << c << "\n";
}

static void opcionDescifrar() {
    int n = leerTamanio();
    Matriz K = leerMatriz(n);
    std::string cripto = leerLinea("Criptograma: ");

    Matriz Kinv;
    if (!inversaGaussJordan(K, MODULO, Kinv)) {
        std::cout << "La clave NO es invertible mod 26: no se puede descifrar.\n";
        return;
    }
    imprimirMatriz(Kinv, "K^-1 (mod 26):");

    // P = K^-1 * C, bloque a bloque.
    std::string cn = normalizar(cripto);
    while (cn.size() % n != 0) cn += 'X';
    std::string claro = aplicarHill(Kinv, cn);
    std::cout << "Texto claro: " << claro << "\n";
}

static void opcionValidar() {
    int n = leerTamanio();
    Matriz K = leerMatriz(n);
    imprimirMatriz(K, "K:");

    int det = determinante(K, MODULO);
    int g = mcd(det, MODULO);
    std::cout << "det(K) mod 26 = " << det << "\n";
    std::cout << "mcd(" << det << ", 26) = " << g << "\n";

    int inv = inversoModular(det, MODULO);
    if (inv == -1) {
        std::cout << "El determinante NO tiene inverso mod 26 -> clave NO valida.\n";
        std::cout << "(Un det par comparte el factor 2 con 26; un multiplo de 13 comparte el 13.)\n";
        return;
    }
    std::cout << "Inverso del det: " << det << "^-1 = " << inv << " (mod 26)\n";

    Matriz Kinv;
    if (inversaGaussJordan(K, MODULO, Kinv)) {
        imprimirMatriz(Kinv, "K^-1 (mod 26):");
        Matriz I = multiplicar(K, Kinv, MODULO);
        imprimirMatriz(I, "Comprobacion K * K^-1 (mod 26):");
        std::cout << "Clave VALIDA.\n";
    }
}

static void opcionCriptoanalisis() {
    int n = leerTamanio();
    std::string claro = leerLinea("Texto claro conocido: ");
    std::string cripto = leerLinea("Criptograma correspondiente: ");

    ResultadoCriptoanalisis r = criptoanalisis(claro, cripto, n);
    separador();
    if (!r.exito) {
        std::cout << "No se pudo recuperar la clave.\n";
        if (r.inconsistente) {
            std::string resp = leerLinea("Buscar correcciones de UNA letra? (s/n): ");
            if (!resp.empty() && (resp[0] == 's' || resp[0] == 'S')) {
                std::cout << "Cambiando una letra del criptograma:\n";
                std::vector<Correccion> enCripto = buscarCorreccionUnaLetra(claro, cripto, n, true);
                imprimirCorrecciones(enCripto);
                if (enCripto.empty() && r.columnasContradiccion.size() >= 2) {
                    std::cout << "  (La contradiccion esta en varias columnas y una letra cifrada\n"
                                 "   solo afecta una columna: hacen falta al menos 2 cambios.)\n";
                }
                std::cout << "Cambiando una letra del texto claro:\n";
                imprimirCorrecciones(buscarCorreccionUnaLetra(claro, cripto, n, false));
            }
        }
        return;
    }
    std::cout << "Metodo: " << r.metodo << "\n";
    if (r.claves.size() == 1) {
        imprimirMatriz(r.claves[0], "Clave recuperada K:");
    } else {
        std::cout << "Claves candidatas: " << r.claves.size() << "\n";
        for (size_t i = 0; i < r.claves.size(); i++) {
            std::cout << "Candidata " << (i + 1) << " (det = "
                      << determinante(r.claves[i], MODULO) << "):\n";
            imprimirMatriz(r.claves[i], "");
        }
    }

    // Verificación final: cifrar el texto claro con cada K recuperada.
    std::string cn = normalizar(cripto);
    std::string pn = normalizar(claro);
    int largo = (int)((std::min(pn.size(), cn.size()) / n) * n);
    bool trazaAnterior = MODO_TRAZA;
    MODO_TRAZA = false;
    for (size_t i = 0; i < r.claves.size(); i++) {
        std::string v = aplicarHill(r.claves[i], pn.substr(0, largo));
        std::cout << "Verificacion K" << (i + 1) << ": " << v
                  << (v == cn.substr(0, largo) ? " == criptograma -> OK\n" : " -> FALLA\n");
    }
    MODO_TRAZA = trazaAnterior;
}

int main() {
    int opcion = -1;
    while (opcion != 0) {
        std::cout << "\n=============================================\n";
        std::cout << " ELC107 - Cifrador de Hill y Criptoanalisis\n";
        std::cout << "=============================================\n";
        std::cout << " 1. Cifrar\n";
        std::cout << " 2. Descifrar\n";
        std::cout << " 3. Validar clave\n";
        std::cout << " 4. Criptoanalisis Gauss-Jordan\n";
        std::cout << " 5. Ejecutar casos de prueba automaticos\n";
        std::cout << " 6. Activar/desactivar MODO TRAZA  [actual: "
                  << (MODO_TRAZA ? "ACTIVO" : "inactivo") << "]\n";
        std::cout << " 0. Salir\n";
        opcion = leerEntero("Opcion: ");

        switch (opcion) {
            case 1: opcionCifrar(); break;
            case 2: opcionDescifrar(); break;
            case 3: opcionValidar(); break;
            case 4: opcionCriptoanalisis(); break;
            case 5: ejecutarCasosDePrueba(); break;
            case 6:
                MODO_TRAZA = !MODO_TRAZA;
                std::cout << "MODO TRAZA " << (MODO_TRAZA ? "ACTIVADO" : "DESACTIVADO") << "\n";
                break;
            case 0: std::cout << "Hasta luego.\n"; break;
            default: std::cout << "Opcion invalida.\n";
        }
    }
    return 0;
}
