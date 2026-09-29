# Cifrador de Hill y Criptoanálisis por Gauss-Jordan

ELC107 Criptografía y Seguridad — aplicación de consola en C++17.
Solo librería estándar; toda la matemática (mod, mcd, Euclides extendido,
inverso modular, determinante, Gauss-Jordan modular, TCR) está implementada
desde cero.

## Compilar

Con g++ (MinGW) en Windows:

```
build.bat
```

o con make:

```
make
```

Ambos compilan con `-std=c++17 -static -Wall`, listando cada `.cpp` de forma
explícita (sin comodines, para que funcione igual en Windows), y dejan el
ejecutable en `build/hill.exe`.

**Dev-C++:** Archivo → Nuevo → Proyecto → *Console Application* (C++),
eliminar el `main.cpp` que crea por defecto, agregar todos los archivos de
`src/nucleo`, `src/ataque`, `src/interfaz` y `src/pruebas` al proyecto y en
*Proyecto → Opciones → Compilador → Generación de código* elegir el estándar
`ISO C++17` (o agregar `-std=c++17` en *Parámetros → Compilador C++*).

## Usar

Ejecutar `hill` (o `hill.exe`):

| Opción | Qué hace |
|---|---|
| 1. Cifrar | Elegir n (2, 3, 4), ingresar K fila por fila, texto y letra de relleno (Enter = X). |
| 2. Descifrar | Calcula K⁻¹ por Gauss-Jordan y descifra. El relleno agregado al cifrar NO se quita solo (Hill no distingue relleno de texto real). |
| 3. Validar clave | Muestra det mod 26, mcd(det, 26), inverso del det y K⁻¹. |
| 4. Criptoanálisis | Con texto claro + criptograma conocidos y n, recupera K. |
| 5. Casos de prueba | Ejecuta los casos automáticos con PASA/FALLA. |
| 6. MODO TRAZA | Activa/desactiva el detalle paso a paso. |

Ejemplo de matriz 2x2: en `Fila 1:` escribir `3 3`, en `Fila 2:` escribir `2 5`.

**Alfabeto:** A=0 … Z=25, módulo 26. El texto se normaliza (mayúsculas, se
eliminan espacios y todo lo que no sea A-Z).

**Convención:** `C = K · P`, con cada bloque P de n letras como vector columna.

## MODO TRAZA

Con la traza activa se muestra:

- cada bloque convertido de letras a números;
- cada producto fila·bloque con la suma de productos y el mod 26;
- el determinante (expansión por la primera fila) y el inverso con
  Euclides extendido, con todas las divisiones;
- cada operación de fila de Gauss-Jordan (`F2 <- F2 - 7*F1 (mod 26)`,
  `F1 <-> F2`, `F1 <- 5*F1`) con la matriz aumentada resultante;
- si ninguna entrada de una columna es invertible mod 26, el aviso
  `[ningun pivote era invertible]` en la suma de filas que fabrica un pivote,
  y `[el pivote de la fila... no era invertible]` en el intercambio de filas
  posterior.

La opción 5 corre sin traza para que el resumen sea legible.

## Criptoanálisis (texto claro conocido)

1. Se forman P y C (n x n) con n bloques como columnas: `C = K·P`.
2. Si `mcd(det(P), 26) = 1` → `K = C · P⁻¹ (mod 26)`.
3. Si no, se prueban automáticamente otras combinaciones de n bloques.
4. Si ninguna sirve: como `26 = 2 · 13`, se resuelve `Pᵀ·Kᵀ = Cᵀ` por
   Gauss-Jordan mod 13 y mod 2 por separado (en mod 2 se enumeran todas las
   soluciones de las variables libres), se combinan con el Teorema Chino del
   Resto (`x = 14·a + 13·b mod 26`) y se listan todas las claves con det
   coprimo con 26 que cifran bien el texto conocido.
5. Siempre se verifica cifrando el texto claro con la K recuperada.
6. **Detección de pares inconsistentes** (Rouché-Frobenius): en cada módulo
   primo se compara rango(P) con rango([P|C]). Si el rango aumentado es mayor,
   alguna fila reducida queda `[0 … 0 | c]` con c ≠ 0, es decir "0 = c", y
   **no existe ninguna K**: si existiera una K mod 26, al reducirla mod 13 o
   mod 2 resolvería ese sistema. El programa muestra ambos rangos, la fila
   reducida y qué columnas del sistema dan la contradicción. La columna j
   corresponde a la fila j de K y a la letra j de cada bloque cifrado.
7. **Búsqueda de correcciones de una letra:** si el par es inconsistente, la
   opción 4 ofrece probar cada posición x las otras 25 letras (16 x 25 = 400
   variantes en el caso 4x4), primero en el criptograma y luego en el texto
   claro. Para cada cambio que vuelve consistente el sistema lista cuántas
   claves válidas (det coprimo con 26) quedan y sus determinantes.

## Estructura

```
src/nucleo/                matemática de base y el cifrador
  modular.*                 mod positivo, mcd, Euclides extendido, inverso modular
  matriz.*                  Matriz, producto, determinante, Gauss-Jordan, inversa
  hill.*                     normalizar, cifrar, descifrar

src/ataque/                 criptoanálisis de texto claro conocido
  ataque_directo.*           método directo: P invertible, K = C * P^-1 y verificación
  sistema_modular.*          resolver K*P = C mod 13 y mod 2 por Gauss-Jordan
  inconsistencia.*           rangos y Rouché-Frobenius (detecta pares sin solución)
  combinar_tcr.*             Teorema Chino del Resto (combina soluciones mod 13/mod 2)
  correcciones.*             búsqueda de correcciones de una letra
  criptoanalisis.*           función principal: decide la ruta (directo o modular)

src/interfaz/                consola
  main.cpp                    menú
  traza.*                     MODO TRAZA e impresión de matrices

src/pruebas/                 casos de prueba automáticos (opción 5)
  tests.*

build/hill.exe               ejecutable compilado
docs/README.md                este documento
build.bat, Makefile           en la raíz del proyecto
```

## Estado de los casos de prueba

16 PASA, 0 FALLA, 1 PENDIENTE. El único pendiente es el **6a**, a propósito,
hasta que se cargue la matriz del docente en `src/pruebas/tests.cpp`
(constante `K_OFICIAL_4X4`, marcada con `TODO`, det = 21).

## Hallazgo: el par oficial 4x4 es inconsistente

Par de la guía: `DELAYOPERATIONSU` → `JCOWZLVBDVLEQMXC`.

- **6b** det(P) = 24 (mod 26). P no es invertible: 24 mod 13 = 11 (sí es
  invertible mod 13), pero 24 mod 2 = 0.
- **Mod 13:** rango(P) = rango([P|C]) = 4, así que hay una única solución.
- **Mod 2:** el programa informa

  ```
  Par inconsistente: rango(P mod 2) = 3, rango([P|C] mod 2) = 4 -> no existe K
  Fila 4 reducida: [ 0 0 0 0 | 0 1 1 0 ]
  Contradiccion 0 = c (c != 0) en la(s) columna(s): 2 (0 = 1), 3 (0 = 1)
  ```

  Es decir: **ninguna matriz 4x4 mod 26 cifra ese texto en ese criptograma.**
  La contradicción está en las ecuaciones de las filas 2 y 3 de K (la 2.ª y la
  3.ª letra de cada bloque cifrado). **6c** pasa porque detectar esto es el
  resultado esperado.
- También se verificó aparte (fuera del programa) que el resultado no depende
  de la convención: con `C = P·K` (vector fila), con el texto escrito por
  filas o por columnas y con A = 1 sigue siendo inconsistente mod 2.

**Correcciones de una letra:**

- **Criptograma: ninguna** (6d). Una letra cifrada es un solo número de C y
  afecta una sola columna del sistema. Como la contradicción está en **dos**
  columnas (2 y 3), hacen falta al menos dos cambios en el criptograma.
- **Texto claro: 25.** Cambiar la 4.ª letra (A de DEL**A**) por 12 de las 13
  letras impares (la P, valor 15, no sirve porque hace singular a P mod 13),
  o la 12.ª (I de RAT**I**) por cualquier letra impar (las 13 sirven),
  arregla el sistema, porque una letra del texto claro cambia P, y P
  interviene en todas las columnas. Cada una deja exactamente una clave
  válida. Entre las de la posición 4 aparece una con det = 21 (texto
  `DELDYOPERATIONSU`), pero **eso no es evidencia**: las 12 correcciones de
  esa posición recorren los 12 determinantes invertibles mod 26, así que
  alguna tiene det = 21 por simple conteo. Además "DELAY OPERATIONS" parece
  ser el texto correcto.

**Conclusión:** el par de la guía tiene un error de copia. Si el error está
en el criptograma, hay al menos dos letras mal: una en la 2.ª posición de
algún bloque y otra en la 3.ª (cambiar letras de las posiciones 1 o 4 no toca
las columnas en contradicción). Hay que
confirmar el par correcto con el docente. Con la matriz real (det = 21)
cargada en `tests.cpp`, el caso 6a muestra qué criptograma produce en realidad.
