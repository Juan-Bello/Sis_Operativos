
#ifndef MM_CLASICA_POSIX_H
#define MM_CLASICA_POSIX_H

#include <sys/time.h>
#include <pthread.h>

#define DATA_SIZE (1024*1024*64*3)

//Definicion de variables globales
extern pthread_mutex_t MM_mutex;
extern double MEM_CHUNK[DATA_SIZE];
extern double *mA, *mB, *mC;
extern struct timeval inicio, fin;


// EStructura que contiene variables para almacenar el el ID del hilo, el número de hilos y el tamaño de la matriz
struct parametros {
    int nH;
    int idH;
    int N;
};

void InicioMuestra();
void FinMuestra();
void iniMatrix(int SZ);
void impMatrix(int sz, double *matriz);
void *multiMatrix(void *variables);

#endif
