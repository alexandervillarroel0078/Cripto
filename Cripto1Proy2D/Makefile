CXX = g++
CXXFLAGS = -std=c++17 -static -Wall

SRCS = src/nucleo/modular.cpp \
       src/nucleo/matriz_basica.cpp \
       src/nucleo/gauss_jordan.cpp \
       src/nucleo/hill.cpp \
       src/ataque/ataque_directo.cpp \
       src/ataque/sistema_modular.cpp \
       src/ataque/inconsistencia.cpp \
       src/ataque/combinar_tcr.cpp \
       src/ataque/correcciones.cpp \
       src/ataque/criptoanalisis.cpp \
       src/interfaz/traza.cpp \
       src/interfaz/entrada.cpp \
       src/interfaz/menu.cpp \
       src/pruebas/tests.cpp

hill: build/hill.exe

build/hill.exe: $(SRCS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) -o build/hill.exe $(SRCS)

clean:
	rm -f build/hill.exe

.PHONY: clean hill
