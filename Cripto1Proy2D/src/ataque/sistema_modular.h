// sistema_modular.h
// Contiene: resolucion de K*P = C modulo un primo p por Gauss-Jordan,
//           enumerando las soluciones cuando hay variables libres.
// No hace: no decide que hacer si el sistema es inconsistente (delega en
//          inconsistencia.h) ni combina soluciones mod 13/mod 2 (combinar_tcr.h).
// Lo usa: criptoanalisis.cpp y correcciones.cpp.
#ifndef SISTEMA_MODULAR_H
#define SISTEMA_MODULAR_H

#include <vector>
#include "../nucleo/matriz.h"

// Resultado de resolver el sistema K*P = C modulo un primo p.
struct InfoSistema {
    bool consistente;
    int rango;                  // rango de P mod p
    int rangoAumentada;         // rango de [P|C] mod p
    int libres;                 // variables libres por fila de K
    std::vector<int> columnasContradiccion; // columnas (1..n) donde aparece 0 = c
    bool truncado;              // true si no se enumeraron todas las soluciones
    std::vector<Matriz> soluciones;
};

// Resuelve K*P = C mod p (p primo) usando todos los bloques conocidos.
// Si imprimir es true, muestra el detalle (rango, variables libres, avisos).
// Enumera hasta "limite" soluciones cuando hay variables libres.
InfoSistema resolverModPrimo(const std::vector<int>& numClaro, const std::vector<int>& numCripto,
                             int bloques, int n, int p, long limite, bool imprimir);

#endif
