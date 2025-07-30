# --------------------
# CONFIGURACIÓN
# --------------------
CC = gcc
CFLAGS = -Wall -pthread
BIN_DIR = bin
OBJ_DIR = obj

# Archivos fuente
RECEPTOR_SRC = receptor.c bd.c
SOLICITANTE_SRC = solicitante.c

# Ejecutables
RECEPTOR_EXE = receptor
SOLICITANTE_EXE = solicitante

# --------------------
# REGLAS
# --------------------

all: $(RECEPTOR_EXE) $(SOLICITANTE_EXE)

$(RECEPTOR_EXE): $(RECEPTOR_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(SOLICITANTE_EXE): $(SOLICITANTE_SRC)
	$(CC) $(CFLAGS) -o $@ $^

run-receptor:
	./receptor -p pipeRP -f libros_inicial.txt -s estado_final.txt -v

run-solicitante:
	./solicitante -p pipeRP
run-solicitanteArchivo:
	./solicitante -p pipeRP -i solicitudes.txt
clean:
	rm -f $(RECEPTOR_EXE) $(SOLICITANTE_EXE)

.PHONY: all clean run-receptor run-solicitante

