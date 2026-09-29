# 1er Proyecto

Este proyecto corresponde a la asignatura **ELC107 Criptografía y Seguridad**. El objetivo es implementar y validar un sistema criptográfico o esteganográfico clásico, demostrando dominio matemático y técnico.

## 1. Entregables por grupo

### Informe académico en PDF (máx. 10 MB)

- Introducción teórica y análisis criptográfico del tema.
- Explicación detallada del algoritmo implementado.
- Casos de prueba y depuración paso a paso.
- Evidencia de ejecución (capturas de pantalla, trazas, resultados).
- Reflexión sobre limitaciones y posibles mejoras.

### Aplicación desarrollada (programa ejecutable o código fuente)

- Debe cumplir con los requerimientos técnicos especificados en la guía.
- No debe depender de APIs externas ni librerías prohibidas.
- Debe permitir validación en vivo durante la defensa oral.
- El código debe estar documentado y organizado.

## 2. Evaluación (100 puntos)

| Criterio                                        | Puntos |
|--------------------------------------------------|:------:|
| Análisis teórico e impacto criptográfico          |   25   |
| Funcionalidad y algoritmos de la aplicación       |   25   |
| Casos de prueba y depuración                      |   25   |
| Defensa en vivo y control anti-IA                 |   15   |
| Uso de terminología técnica                       |   10   |

## 3. Disponibilidad

- **Inicio de entrega:** 01 de octubre de 2026
- **Fecha límite:** 08 de octubre de 2026
- **Defensa presencial:** 08 de octubre

## 4. Proyecto 2 – Cifrador de Hill y Criptoanálisis Gauss-Jordan (Grupo D)

**Objetivo:** Implementar un cifrador poligráfico Hill (3x3 o 4x4) y un módulo de criptoanálisis por Gauss-Jordan.

**Entregables:** Informe PDF + aplicación funcional.

**Caso de prueba:** Matriz clave 4x4 con determinante coprimo (21).

- Texto claro: `DELAY OPERATIONSU`
- Criptograma: `JCOW ZLVB DVLE QMXC`

## 5. Lenguajes recomendados por proyecto

| # | Proyecto                          | Lenguajes recomendados                                         |
|---|------------------------------------|------------------------------------------------------------------|
| 1 | Criptoanalizador de Vigenère       | C, C++ (control de cadenas y modularidad), Python (solo si implementa lógica desde cero) |
| 2 | Cifrador de Hill                   | C, C++ (matrices modulares), Java                                |
| 3 | Esteganografía LSB                 | Python, C# (manejo de imágenes), Java                            |
| 4 | Cifrador Afín                      | C, C++, Java                                                     |
| 5 | Doble Transposición Columnar       | C, C++, Java                                                     |
| 6 | Simulador Enigma                   | C, C++, Java                                                     |
| 7 | Rejilla de Cardano                 | C, C++, Java                                                     |

## 6. Reglas generales

- **Lenguaje base obligatorio:** C o C++ para proyectos matemáticos (1, 2, 4, 5, 6, 7).
- **Lenguajes alternativos permitidos:** Python, C#, Java en proyectos multimedia o de interfaz (3).
- **Restricción clave:** No se permite usar librerías externas que resuelvan automáticamente el algoritmo (ej. numpy, cryptography, opencv para esteganografía).
- **Entrega:** El código debe estar documentado y autocontenido.
