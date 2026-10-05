// main.cpp - Cifrado de Hill en consola.
// Solo E/S y presentacion: toda la logica criptografica vive en core/
// (HillCipher, GaussJordan, MatrizMod26, AlfabetoHill).
// Compatible con g++ y Dev-C++ (MinGW) usando -std=c++11.

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "core/Tipos.h"
#include "core/AlfabetoHill.h"
#include "core/MatrizMod26.h"
#include "core/GaussJordan.h"
#include "core/HillCipher.h"

using namespace std;

// ---------------------------------------------------------------------------
// Datos del caso oficial
// ---------------------------------------------------------------------------
static Mat claveOficial4() {
    int d[4][4] = { {8, 6, 9, 5}, {6, 9, 5, 10}, {5, 8, 4, 9}, {10, 6, 11, 4} };
    Mat K(4, Vec(4, 0));
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) K[i][j] = d[i][j];
    return K;
}
static const char* TEXTO_OFICIAL = "DELAYOPERATIONSU";
static const char* CRIPTO_OFICIAL = "JCOWZLVBDVLEQMXC";

// ---------------------------------------------------------------------------
// Entrada de datos
// ---------------------------------------------------------------------------

// Lee una linea; si se acaba la entrada (Ctrl+Z / Ctrl+D) termina el programa.
static string leerLinea(const string& indicacion) {
    cout << indicacion;
    string s;
    if (!getline(cin, s)) {
        cout << "\nFin de la entrada. Saliendo.\n";
        exit(0);
    }
    return s;
}

// Intenta interpretar toda la cadena como un entero.
static bool aEntero(const string& s, int& valor) {
    istringstream in(s);
    long long v;
    char resto;
    if (!(in >> v)) return false;
    if (in >> resto) return false;           // hay basura despues del numero
    if (v < -1000000 || v > 1000000) return false;
    valor = (int)v;
    return true;
}

// Pide un entero (repite hasta que sea valido).
static int pedirEntero(const string& indicacion) {
    for (;;) {
        int v;
        if (aEntero(leerLinea(indicacion), v)) return v;
        cout << "  Entrada invalida: escriba un numero entero.\n";
    }
}

// Pide n = 3 o 4.
static int pedirTamano() {
    for (;;) {
        int n = pedirEntero("Tamano de la clave n (3 o 4): ");
        if (n == 3 || n == 4) return n;
        cout << "  n debe ser 3 o 4.\n";
    }
}

// Pide una matriz de f x c, fila por fila (numeros separados por espacios).
// Los valores se reducen mod 26.
static Mat pedirMatriz(const string& nombre, int f, int c) {
    cout << "Ingrese " << nombre << " (" << f << "x" << c
         << "), fila por fila, " << c << " numeros por fila:\n";
    Mat M(f, Vec(c, 0));
    for (int i = 0; i < f; ++i) {
        for (;;) {
            ostringstream ind;
            ind << "  Fila " << (i + 1) << ": ";
            istringstream in(leerLinea(ind.str()));
            Vec fila;
            string tok;
            bool ok = true;
            while (in >> tok) {
                int v;
                if (!aEntero(tok, v)) { ok = false; break; }
                fila.push_back(MatrizMod26::mod(v));
            }
            if (ok && (int)fila.size() == c) { M[i] = fila; break; }
            cout << "  Fila invalida: se esperan exactamente " << c
                 << " numeros enteros.\n";
        }
    }
    return M;
}

// ---------------------------------------------------------------------------
// Salida de datos
// ---------------------------------------------------------------------------

static void imprimirMatriz(const Mat& M, const string& sangria = "  ") {
    for (size_t i = 0; i < M.size(); ++i) {
        cout << sangria << "[";
        for (size_t j = 0; j < M[i].size(); ++j)
            cout << setw(3) << M[i][j];
        cout << " ]\n";
    }
}

// Imprime la matriz aumentada [A | B] separando las dos mitades con '|'.
static void imprimirAumentada(const Mat& M) {
    for (size_t i = 0; i < M.size(); ++i) {
        size_t mitad = M[i].size() / 2;
        cout << "    [";
        for (size_t j = 0; j < M[i].size(); ++j) {
            if (j == mitad) cout << " |";
            cout << setw(3) << M[i][j];
        }
        cout << " ]\n";
    }
}

static void imprimirPasos(const vector<Paso>& pasos) {
    for (size_t i = 0; i < pasos.size(); ++i) {
        cout << "  Paso " << (i + 1) << ": " << pasos[i].descripcion << "\n";
        imprimirAumentada(pasos[i].aumentada);
    }
}

static void titulo(const string& t) {
    cout << "\n=== " << t << " ===\n";
}

// Muestra el determinante y si la matriz es invertible mod 26.
static bool mostrarInvertibilidad(const Mat& K, const string& nombre = "K") {
    int det = MatrizMod26::determinante(K);
    bool inv = MatrizMod26::esInvertible(K);
    cout << "  det(" << nombre << ") mod 26 = " << det << "\n";
    if (inv)
        cout << "  " << nombre << " es invertible mod 26 (mcd(det,26)=1, det^-1 = "
             << MatrizMod26::inversoMod(det, MOD) << ").\n";
    else
        cout << "  " << nombre << " NO es invertible mod 26 (mcd(det,26) != 1).\n";
    return inv;
}

// Convierte un texto en bloques de n letras separados por espacio.
static string aBloques(const string& texto, int n) {
    string r;
    for (size_t i = 0; i < texto.size(); ++i) {
        if (i > 0 && i % n == 0) r += ' ';
        r += texto[i];
    }
    return r;
}

// Muestra cada bloque: letras -> numeros -> K*bloque -> letras cifradas.
static void mostrarBloques(const string& limpio, const Mat& K) {
    int n = (int)K.size();
    vector<Vec> bloques = HillCipher::dividirBloques(HillCipher::textoAVector(limpio), n);
    for (size_t b = 0; b < bloques.size(); ++b) {
        Vec c = MatrizMod26::multiplicar(K, bloques[b]);
        cout << "  Bloque " << (b + 1) << ": " << limpio.substr(b * n, n) << " = (";
        for (int i = 0; i < n; ++i) cout << (i ? "," : "") << bloques[b][i];
        cout << ")  ->  K*p = (";
        for (int i = 0; i < n; ++i) cout << (i ? "," : "") << c[i];
        cout << ")  ->  " << HillCipher::vectorATexto(c) << "\n";
    }
}

// ---------------------------------------------------------------------------
// Operaciones (reutilizadas por el menu, el caso oficial y las pruebas)
// ---------------------------------------------------------------------------

// Cifra 'texto' con K mostrando todo. Devuelve el criptograma ("" si hay error).
// Si 'preguntar' es true y la longitud no es multiplo de n, ofrece rellenar con X.
static string operacionCifrar(const string& texto, const Mat& K, bool preguntar) {
    int n = (int)K.size();
    titulo("Cifrado");
    cout << "Clave K:\n";
    imprimirMatriz(K);
    if (!mostrarInvertibilidad(K)) {
        cout << "ERROR: no se puede cifrar con una K no invertible "
                "(el mensaje no podria descifrarse).\n";
        return "";
    }

    string limpio = AlfabetoHill::limpiar(texto);
    if (limpio.empty()) {
        cout << "ERROR: el texto esta vacio (solo se admiten letras A-Z).\n";
        return "";
    }
    if (limpio.size() != texto.size())
        cout << "  Nota: se ignoraron espacios, signos y caracteres no A-Z.\n";

    if ((int)limpio.size() % n != 0) {
        int faltan = n - (int)(limpio.size() % n);
        cout << "  La longitud (" << limpio.size() << ") no es multiplo de n=" << n
             << "; faltan " << faltan << " letra(s).\n";
        if (preguntar) {
            string r = leerLinea("  Rellenar con 'X'? (s/n): ");
            if (r.empty() || (r[0] != 's' && r[0] != 'S')) {
                cout << "  Cifrado cancelado.\n";
                return "";
            }
        }
        limpio = AlfabetoHill::rellenar(limpio, n, 'X');
        cout << "  Texto con relleno: " << aBloques(limpio, n) << "\n";
    }

    cout << "Texto plano : " << aBloques(limpio, n) << "\n";
    mostrarBloques(limpio, K);
    string cripto = HillCipher::cifrar(limpio, K);
    cout << "Criptograma : " << aBloques(cripto, n) << "\n";
    return cripto;
}

// Descifra con K mostrando K^-1. Devuelve el texto ("" si hay error).
static string operacionDescifrar(const string& cripto, const Mat& K) {
    int n = (int)K.size();
    titulo("Descifrado");
    cout << "Clave K:\n";
    imprimirMatriz(K);
    if (!mostrarInvertibilidad(K)) {
        cout << "ERROR: no se puede descifrar con una K no invertible.\n";
        return "";
    }
    string limpio = AlfabetoHill::limpiar(cripto);
    if (limpio.empty()) {
        cout << "ERROR: el criptograma esta vacio (solo se admiten letras A-Z).\n";
        return "";
    }
    if ((int)limpio.size() % n != 0) {
        cout << "ERROR: la longitud del criptograma (" << limpio.size()
             << ") no es multiplo de n=" << n << ".\n";
        return "";
    }
    vector<Paso> pasos;
    Mat Kinv = GaussJordan::invertir(K, pasos);
    cout << "K^-1 (Gauss-Jordan):\n";
    imprimirMatriz(Kinv);
    string plano = HillCipher::descifrar(limpio, K);
    cout << "Criptograma : " << aBloques(limpio, n) << "\n";
    cout << "Texto plano : " << aBloques(plano, n) << "\n";
    return plano;
}

// Inversa con todos los pasos y verificacion K*K^-1 = I. Devuelve true si invertible.
static bool operacionInversa(const Mat& K) {
    titulo("Inversa por Gauss-Jordan");
    cout << "Matriz K:\n";
    imprimirMatriz(K);
    mostrarInvertibilidad(K);
    vector<Paso> pasos;
    Mat Kinv = GaussJordan::invertir(K, pasos);
    imprimirPasos(pasos);
    if (Kinv.empty()) {
        cout << "\nK no tiene inversa mod 26; no hay nada que verificar.\n";
        return false;
    }
    cout << "\nK^-1:\n";
    imprimirMatriz(Kinv);
    Mat prod = MatrizMod26::multiplicar(K, Kinv);
    cout << "Verificacion K * K^-1 mod 26:\n";
    imprimirMatriz(prod);
    bool esI = (prod == MatrizMod26::identidad((int)K.size()));
    cout << (esI ? "K * K^-1 = I  -> CORRECTO\n" : "K * K^-1 != I -> INCORRECTO\n");
    return esI;
}

// Criptoanalisis: K = C * P^-1. Devuelve la K recuperada (vacia si P no es invertible).
static Mat operacionCriptoanalisis(const Mat& P, const Mat& C) {
    titulo("Criptoanalisis con texto plano conocido");
    cout << "P (bloques de texto plano como columnas):\n";
    imprimirMatriz(P);
    cout << "C (bloques de criptograma como columnas):\n";
    imprimirMatriz(C);
    cout << "Se cumple C = K * P, por lo tanto K = C * P^-1 (mod 26).\n";
    mostrarInvertibilidad(P, "P");
    vector<Paso> pasos;
    Mat K = GaussJordan::recuperarClave(P, C, pasos);
    cout << "Inversion de P por Gauss-Jordan:\n";
    imprimirPasos(pasos);
    if (K.empty()) {
        cout << "\nP no es invertible mod 26: con estos bloques no se puede "
                "recuperar K. Use otro texto conocido.\n";
        return K;
    }
    cout << "\nK recuperada = C * P^-1:\n";
    imprimirMatriz(K);
    return K;
}

// Arma P o C a partir de un texto de n*n letras: el bloque j es la columna j.
static Mat matrizDesdeTexto(const string& limpio, int n) {
    vector<Vec> bloques = HillCipher::dividirBloques(HillCipher::textoAVector(limpio), n);
    Mat M(n, Vec(n, 0));
    for (int j = 0; j < n; ++j)
        for (int i = 0; i < n; ++i) M[i][j] = bloques[j][i];
    return M;
}

// ---------------------------------------------------------------------------
// Opciones del menu
// ---------------------------------------------------------------------------

// Criptoanalisis con texto plano conocido de longitud m*n (m >= n bloques).
// Busca n bloques (en orden lexicografico, empezando por los primeros) cuya matriz P
// sea invertible mod 26, recupera K = C*P^-1 y la verifica cifrando TODO el texto plano.
// Devuelve K (vacia si ninguna combinacion sirve o K no reproduce el cifrado).
// Si 'usados' no es nulo, recibe los indices (base 0) de los bloques elegidos.
static Mat operacionCriptoanalisisTexto(const string& tp, const string& tc, int n,
                                        vector<int>* usados = 0) {
    const long long MAX_COMBINACIONES = 100000;
    vector<Vec> bp = HillCipher::dividirBloques(HillCipher::textoAVector(tp), n);
    vector<Vec> bc = HillCipher::dividirBloques(HillCipher::textoAVector(tc), n);
    int m = (int)bp.size();
    titulo("Criptoanalisis con texto plano conocido (texto de " +
           to_string(tp.size()) + " letras, " + to_string(m) + " bloques)");

    // Combinaciones de n bloques, en orden lexicografico.
    vector<int> idx(n);
    for (int i = 0; i < n; ++i) idx[i] = i;
    long long probadas = 0;
    bool encontrada = false;
    Mat P, C;
    for (;;) {
        ++probadas;
        P = Mat(n, Vec(n, 0));
        C = Mat(n, Vec(n, 0));
        for (int j = 0; j < n; ++j)
            for (int i = 0; i < n; ++i) { P[i][j] = bp[idx[j]][i]; C[i][j] = bc[idx[j]][i]; }
        if (MatrizMod26::esInvertible(P)) { encontrada = true; break; }
        if (probadas >= MAX_COMBINACIONES) break;
        // siguiente combinacion
        int i = n - 1;
        while (i >= 0 && idx[i] == m - n + i) --i;
        if (i < 0) break;
        ++idx[i];
        for (int j = i + 1; j < n; ++j) idx[j] = idx[j - 1] + 1;
    }

    if (!encontrada) {
        cout << "Se probaron " << probadas << " combinacion(es) de " << n << " bloques"
             << (probadas >= MAX_COMBINACIONES ? " (limite alcanzado)" : "") << ".\n"
             << "ERROR: ninguna combinacion de bloques da una P invertible mod 26; "
                "no se puede recuperar K. Use otro texto conocido.\n";
        return Mat();
    }

    cout << "Bloques usados (numerados desde 1): ";
    for (int j = 0; j < n; ++j) cout << (j ? ", " : "") << idx[j] + 1;
    cout << "\n";
    if (probadas > 1)
        cout << "Las combinaciones anteriores (" << probadas - 1
             << ", empezando por los primeros " << n
             << " bloques) daban P no invertible mod 26.\n";

    Mat K = operacionCriptoanalisis(P, C);
    if (K.empty()) return K;

    bool ok = (HillCipher::cifrar(tp, K) == tc);
    cout << "Verificacion: cifrar los " << tp.size() << " caracteres del texto plano con K "
         << (ok ? "reproduce el criptograma completo -> CORRECTO\n"
                : "NO reproduce el criptograma -> INCORRECTO\n");
    if (!ok) return Mat();
    if (usados) *usados = idx;
    return K;
}

static void menuCifrar() {
    int n = pedirTamano();
    Mat K = pedirMatriz("la clave K", n, n);
    string texto = leerLinea("Texto plano: ");
    operacionCifrar(texto, K, true);
}

static void menuDescifrar() {
    int n = pedirTamano();
    Mat K = pedirMatriz("la clave K", n, n);
    string cripto = leerLinea("Criptograma: ");
    operacionDescifrar(cripto, K);
}

static void menuInversa() {
    int n = pedirTamano();
    Mat K = pedirMatriz("la matriz K", n, n);
    operacionInversa(K);
}

static void menuCriptoanalisis() {
    int n = pedirTamano();
    cout << "Datos conocidos: 1) matrices P y C   2) texto plano y criptograma ("
         << n * n << " letras)\n";
    int modo = pedirEntero("Opcion: ");
    Mat P, C;
    if (modo == 2) {
        string tp = AlfabetoHill::limpiar(leerLinea("Texto plano conocido: "));
        string tc = AlfabetoHill::limpiar(leerLinea("Criptograma correspondiente: "));
        if (tp.size() != tc.size() || (int)tp.size() % n != 0 || (int)tp.size() < n * n) {
            cout << "ERROR: ambos textos deben tener la misma longitud, multiplo de n=" << n
                 << " y de al menos " << n * n << " letras (n bloques de n letras).\n";
            return;
        }
        operacionCriptoanalisisTexto(tp, tc, n);
        return;
    }
    P = pedirMatriz("la matriz P", n, n);
    C = pedirMatriz("la matriz C", n, n);
    operacionCriptoanalisis(P, C);
}

static void casoOficial() {
    titulo("Caso oficial 4x4");
    Mat K = claveOficial4();
    string cripto = operacionCifrar(TEXTO_OFICIAL, K, false);
    cout << "Esperado    : " << CRIPTO_OFICIAL << "  -> "
         << (cripto == CRIPTO_OFICIAL ? "COINCIDE" : "NO COINCIDE") << "\n";
    string plano = operacionDescifrar(cripto, K);
    cout << "Original    : " << TEXTO_OFICIAL << "  -> "
         << (plano == TEXTO_OFICIAL ? "COINCIDE" : "NO COINCIDE") << "\n";
    Mat Kr = operacionCriptoanalisis(matrizDesdeTexto(TEXTO_OFICIAL, 4),
                                     matrizDesdeTexto(cripto, 4));
    cout << "K recuperada == K original? " << (Kr == K ? "SI" : "NO") << "\n";
}

// ---------------------------------------------------------------------------
// Pruebas automaticas
// ---------------------------------------------------------------------------

static int fallos = 0;

static void reportar(const string& nombre, bool ok) {
    cout << (ok ? "[OK]    " : "[FALLO] ") << nombre << "\n";
    if (!ok) ++fallos;
}

// Silencia cout mientras viva (las operaciones imprimen mucho).
class SilenciarSalida {
    streambuf* previo;
    ostringstream descarte;
public:
    SilenciarSalida() : previo(cout.rdbuf(descarte.rdbuf())) {}
    ~SilenciarSalida() { cout.rdbuf(previo); }
};

static void ejecutarPruebas() {
    titulo("Pruebas automaticas");
    fallos = 0;

    // 1. Caso oficial 4x4
    {
        Mat K = claveOficial4();
        string c = HillCipher::cifrar(TEXTO_OFICIAL, K);
        reportar("Caso oficial 4x4: DELAYOPERATIONSU -> JCOWZLVBDVLEQMXC (obtenido: "
                 + c + ")", c == CRIPTO_OFICIAL);
    }
    // 2. Cifrar -> descifrar devuelve el original (4x4)
    {
        Mat K = claveOficial4();
        string t = "CRIPTOGRAFIAHILL";   // 16 letras
        string d = HillCipher::descifrar(HillCipher::cifrar(t, K), K);
        reportar("Cifrar->descifrar 4x4 devuelve el original", d == t);
    }
    // 3. Caso 3x3
    {
        Mat K(3, Vec(3, 0));
        int d[3][3] = { {3, 10, 20}, {20, 9, 17}, {9, 4, 15} };
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) K[i][j] = d[i][j];
        string t = "HOLAMUNDO";
        string c = HillCipher::cifrar(t, K);
        string p = HillCipher::descifrar(c, K);
        reportar("Caso 3x3: K invertible y cifrar->descifrar (" + t + " -> " + c + ")",
                 MatrizMod26::esInvertible(K) && p == t && c != t);
    }
    // 4. Inversa verificada: K * K^-1 = I
    {
        Mat K = claveOficial4();
        Mat Ki = GaussJordan::invertir(K);
        reportar("Inversa 4x4: K * K^-1 = I",
                 !Ki.empty() && MatrizMod26::multiplicar(K, Ki) == MatrizMod26::identidad(4));
    }
    // 5. Criptoanalisis: K recuperada == K original
    {
        Mat K = claveOficial4();
        string t = TEXTO_OFICIAL;
        Mat Kr = GaussJordan::recuperarClave(matrizDesdeTexto(t, 4),
                     matrizDesdeTexto(HillCipher::cifrar(t, K), 4));
        reportar("Criptoanalisis 4x4: K recuperada == K original", Kr == K);
    }
    // 6. Matriz singular: debe avisar sin romper
    {
        Mat S(3, Vec(3, 0));
        int d[3][3] = { {1, 2, 3}, {2, 4, 6}, {5, 7, 9} };   // F2 = 2*F1
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) S[i][j] = d[i][j];
        bool detectada = false, sinCrash = true;
        {
            SilenciarSalida mudo;
            detectada = !MatrizMod26::esInvertible(S) && GaussJordan::invertir(S).empty();
            string r = operacionDescifrar("ABCDEFGHI", S);   // debe avisar y devolver ""
            if (!r.empty()) sinCrash = false;
            if (operacionInversa(S)) sinCrash = false;
            if (!operacionCriptoanalisis(S, S).empty()) sinCrash = false;
        }
        reportar("Matriz singular: se detecta y las operaciones avisan sin romper",
                 detectada && sinCrash);
    }
    // 7. Validaciones de entrada
    {
        Mat K = claveOficial4();
        bool ok;
        {
            SilenciarSalida mudo;
            ok = operacionCifrar("", K, false).empty()                 // texto vacio
              && operacionDescifrar("ABCDE", K).empty();               // longitud no multiplo
        }
        reportar("Validaciones: texto vacio y longitud no multiplo de n", ok);
    }

    // 8. Cifrar con K singular: avisa y NO cifra
    {
        Mat S(3, Vec(3, 0));
        int d[3][3] = { {1, 2, 3}, {2, 4, 6}, {5, 7, 9} };
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) S[i][j] = d[i][j];
        string r = "?";
        {
            SilenciarSalida mudo;
            r = operacionCifrar("HOLAMUNDO", S, false);
        }
        reportar("Cifrar con K singular: error y no se cifra", r.empty());
    }
    // 9. Criptoanalisis con texto largo: los primeros bloques dan P singular
    {
        Mat K(3, Vec(3, 0));
        int d[3][3] = { {3, 10, 20}, {20, 9, 17}, {9, 4, 15} };
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) K[i][j] = d[i][j];
        string t = "HOLHOLHOLAMUNDOXYZ";            // los 3 primeros bloques son iguales
        string c = HillCipher::cifrar(t, K);
        bool primerosSingular = !MatrizMod26::esInvertible(
            matrizDesdeTexto(t.substr(0, 9), 3));
        Mat Kr;
        vector<int> usados;
        {
            SilenciarSalida mudo;
            Kr = operacionCriptoanalisisTexto(t, c, 3, &usados);
        }
        reportar("Criptoanalisis con texto largo: primeros bloques singulares, "
                 "se usan otros y K recuperada == K original",
                 primerosSingular && Kr == K && usados.size() == 3 && usados[0] == 0
                 && !(usados[1] == 1 && usados[2] == 2));
        // Ninguna combinacion sirve: todos los bloques iguales
        Mat Kn;
        {
            SilenciarSalida mudo;
            string t2 = "HOLHOLHOLHOL";
            Kn = operacionCriptoanalisisTexto(t2, HillCipher::cifrar(t2, K), 3);
        }
        reportar("Criptoanalisis con texto largo sin P invertible: se avisa", Kn.empty());
    }

    cout << "\nResultado: " << (fallos == 0 ? "TODAS LAS PRUEBAS OK" : "HAY PRUEBAS CON FALLO")
         << " (fallos: " << fallos << ")\n";
}

// ---------------------------------------------------------------------------
// Programa principal
// ---------------------------------------------------------------------------

int main() {
    for (;;) {
        cout << "\n===== CIFRADO DE HILL - CONSOLA =====\n"
             << " 1. Cifrar\n"
             << " 2. Descifrar\n"
             << " 3. Inversa de K (Gauss-Jordan paso a paso)\n"
             << " 4. Criptoanalisis con texto plano conocido\n"
             << " 5. Cargar caso oficial 4x4\n"
             << " 6. Ejecutar pruebas automaticas\n"
             << " 0. Salir\n";
        string op = leerLinea("Opcion: ");
        int o;
        if (!aEntero(op, o)) { cout << "Opcion invalida.\n"; continue; }
        switch (o) {
            case 1: menuCifrar(); break;
            case 2: menuDescifrar(); break;
            case 3: menuInversa(); break;
            case 4: menuCriptoanalisis(); break;
            case 5: casoOficial(); break;
            case 6: ejecutarPruebas(); break;
            case 0: cout << "Hasta luego.\n"; return 0;
            default: cout << "Opcion invalida.\n";
        }
    }
}
