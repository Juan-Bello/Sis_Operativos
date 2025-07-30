#ifndef MM_CLASICA_OPENMP_H
#define MM_CLASICA_OPENMP_H

#include <sys/time.h>

// Definicion variables globales
extern struct timeval inicio, fin;

void InicioMuestra();
void FinMuestra();
void impMatrix(size_t *matrix, int D);
void iniMatrix(size_t *m1, size_t *m2, int D);
void multiMatrix(size_t *mA, size_t *mB, size_t *mC, int D);

#endif
