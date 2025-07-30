#ifndef MM_CLASICA_FORK_H
#define MM_CLASICA_FORK_H

#include <sys/time.h>

// Definicion de las variables globales
extern struct timeval inicio, fin;

void InicioMuestra();
void FinMuestra();
void multiMatrix(double *mA, double *mB, double *mC, int D, int filaI, int filaF);
void impMatrix(double *matrix, int D);
void iniMatrix(double *mA, double *mB, int D);

#endif
