// criptoanalisis.cpp
#include "criptoanalisis.h"
#include "ataque_directo.h"
#include "sistema_modular.h"
#include "combinar_tcr.h"
#include "../nucleo/hill.h"
#include "../nucleo/modular.h"
#include "../interfaz/traza.h"
#include <iostream>
#include <algorithm>

// Limite de soluciones a enumerar por modulo (evita esperas infinitas si
// el texto conocido es muy corto y hay demasiadas variables libres).
const long LIMITE_SOLUCIONES = 200000;

// Arma el diagnostico de "res" cuando mod 13 o mod 2 no tienen solucion.
static void marcarInconsistente(ResultadoCriptoanalisis& res, const InfoSistema& info13, const InfoSistema& info2) {
    const InfoSistema& malo = !info13.consistente ? info13 : info2;
    res.inconsistente = true;
    res.moduloInconsistente = !info13.consistente ? 13 : 2;
    res.rangoP = malo.rango;
    res.rangoPC = malo.rangoAumentada;
    res.columnasContradiccion = malo.columnasContradiccion;
}

// Explica por que hay varias candidatas cuando sol13.size()*sol2.size() > 1.
static void explicarVariasCandidatas(const InfoSistema& info13, const InfoSistema& info2,
                                     size_t nSol13, size_t nSol2) {
    std::cout << "\n  Por que hay varias candidatas?\n";
    if (info2.libres > 0) {
        std::cout << "  - det(P) es par: P NO es invertible mod 2. El sistema mod 2 tiene "
                  << info2.libres << " variable(s) libre(s)\n"
                  << "    por cada fila de K, y cada una puede valer 0 o 1: hay "
                  << nSol2 << " soluciones mod 2.\n";
    }
    if (info13.libres > 0) {
        std::cout << "  - det(P) es multiplo de 13: el sistema mod 13 tiene " << nSol13
                  << " soluciones.\n";
    } else {
        std::cout << "  - Mod 13, P si es invertible: la solucion mod 13 es unica.\n";
    }
    std::cout << "  - Cada par (solucion mod 13, solucion mod 2) da, por TCR, una K mod 26\n"
              << "    distinta que cumple K*P = C: el texto conocido NO alcanza para\n"
              << "    distinguirlas. Se descartan las que tienen det no coprimo con 26\n"
              << "    (no servirian para descifrar).\n\n";
}

// Metodo por modulos primos + Teorema Chino del Resto.
// 26 = 2 * 13 con 2 y 13 primos. Mod un primo todo numero distinto de 0 es
// invertible, asi que Gauss-Jordan funciona "como en los reales". Resolvemos
// K*P = C mod 13 y mod 2 por separado y luego combinamos.
static void intentarMetodoModular(ResultadoCriptoanalisis& res, const std::vector<int>& numClaro,
                                  const std::vector<int>& numCripto, int bloques, int n,
                                  const std::string& claro, const std::string& cripto) {
    separador();
    res.metodo = "mod 13 / mod 2 + TCR";
    std::cout << "  Resolviendo K*P = C por separado mod 13 y mod 2 (26 = 2*13).\n";
    if (res.detPrimerosBloques >= 0) {
        std::cout << "  det(P) = " << res.detPrimerosBloques << " -> mod 13: "
                  << mod(res.detPrimerosBloques, 13) << ", mod 2: "
                  << mod(res.detPrimerosBloques, 2) << "\n";
    }

    InfoSistema info13 = resolverModPrimo(numClaro, numCripto, bloques, n, 13, LIMITE_SOLUCIONES, true);
    InfoSistema info2 = resolverModPrimo(numClaro, numCripto, bloques, n, 2, LIMITE_SOLUCIONES, true);

    if (!info13.consistente || !info2.consistente) {
        // Si K*P = C tuviera solucion mod 26, reduciendola tendriamos una
        // solucion mod 13 y otra mod 2. Como en algun modulo no hay, no existe K.
        marcarInconsistente(res, info13, info2);
        std::cout << "  Resultado: el par texto/cripto NO proviene de ningun cifrado de Hill "
                  << n << "x" << n << " mod 26\n"
                  << "  (si existiera K, reduciendola mod " << res.moduloInconsistente
                  << " resolveria este sistema).\n";
        return;
    }

    const std::vector<Matriz>& sol13 = info13.soluciones;
    const std::vector<Matriz>& sol2 = info2.soluciones;
    std::cout << "  TCR: x = 14*a + 13*b (mod 26), con a = x mod 13, b = x mod 2\n";

    if (sol13.size() * sol2.size() > 1) {
        explicarVariasCandidatas(info13, info2, sol13.size(), sol2.size());
    }

    res.claves = combinarTCR(sol13, sol2, n, claro, cripto);

    std::cout << "  Combinaciones probadas: " << sol13.size() * sol2.size()
              << ", candidatas validas (det coprimo con 26 y cifran bien): "
              << res.claves.size() << "\n";

    res.exito = !res.claves.empty();
}

ResultadoCriptoanalisis criptoanalisis(const std::string& claroOriginal,
                                       const std::string& criptoOriginal, int n) {
    ResultadoCriptoanalisis res;
    res.exito = false;
    res.metodo = "";
    res.detPrimerosBloques = -1;
    res.inconsistente = false;
    res.moduloInconsistente = 0;
    res.rangoP = 0;
    res.rangoPC = 0;

    std::string claro = normalizar(claroOriginal);
    std::string cripto = normalizar(criptoOriginal);

    // Solo sirven bloques completos que esten en ambos textos.
    int bloques = (int)(std::min(claro.size(), cripto.size()) / n);
    claro = claro.substr(0, bloques * n);
    cripto = cripto.substr(0, bloques * n);

    std::cout << "  Texto claro: " << claro << "\n";
    std::cout << "  Criptograma: " << cripto << "\n";
    std::cout << "  Bloques completos de " << n << " letras disponibles: " << bloques << "\n";

    if (bloques == 0) {
        std::cout << "  No hay ningun bloque completo: imposible atacar.\n";
        return res;
    }

    std::vector<int> numClaro = textoANumeros(claro);
    std::vector<int> numCripto = textoANumeros(cripto);

    // 1) Metodo directo: buscar n bloques cuya P sea invertible mod 26.
    ResultadoDirecto directo = intentarAtaqueDirecto(numClaro, numCripto, bloques, n, claro, cripto);
    res.detPrimerosBloques = directo.detPrimerosBloques;
    if (directo.exito) {
        res.exito = true;
        res.metodo = "directo";
        res.claves.push_back(directo.clave);
        return res;
    }

    // 2) Metodo por modulos primos + Teorema Chino del Resto.
    intentarMetodoModular(res, numClaro, numCripto, bloques, n, claro, cripto);
    return res;
}
