GCC = gcc
CFLAGS = -lm
FOPENMP = -fopenmp -O3
POSIX = -lpthread

PROGRAMAS = mmClasicaOpenMP mmClasicaFork mmClasicaPosix

all: $(PROGRAMAS)

mmClasicaPosix: mmClasicaPosix.c mainPosix.c mmClasicaPosix.h
	$(GCC) $(CFLAGS) mmClasicaPosix.c mainPosix.c -o $@ $(POSIX)

mmClasicaFork: mmClasicaFork.c mainFork.c mmClasicaFork.h
	$(GCC) $(CFLAGS) mmClasicaFork.c mainFork.c -o $@

mmClasicaOpenMP: mmClasicaOpenMP.c mainOpenMP.c mmClasicaOpenMP.h
	$(GCC) $(CFLAGS) mmClasicaOpenMP.c mainOpenMP.c -o $@ $(FOPENMP)

clean:
	$(RM) $(PROGRAMAS)
