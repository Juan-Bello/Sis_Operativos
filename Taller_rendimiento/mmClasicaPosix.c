#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mmClasicaPosix.h"

#define DATA_SIZE (1024*1024*64*3) 

// Declarar variables globales para el mutex, memoria de matrices y medicion de tiempo
pthread_mutex_t MM_mutex;
double MEM_CHUNK[DATA_SIZE];
double *mA, *mB, *mC;

struct timeval inicio, fin;

// FUncion que inicia medicion de tiempo
void InicioMuestra(){
	// Obtener el tiempo actual y almacenar
	gettimeofday(&inicio, (void *)0);
}

// Funcion que termina la medicion de tiempo
void FinMuestra(){
// Calcular la diferencia entre el primer tiempo y el segundo
	gettimeofday(&fin, (void *)0);
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double) (fin.tv_sec*1000000 + fin.tv_usec);
	//IMprimir resultado
	printf("%9.0f \n", tiempo);
}

// FUncion para inicializar las matrices con datos aleatorios
void iniMatrix(int SZ){ 
	for(int i = 0; i < SZ*SZ; i++){
	// En cada posicion, se saca un numero aleatorio hasta el 10 y se pone en las 2 matrices. C se inicializa con todo en 0
			mA[i] = rand() % 10; 
			mB[i] = rand() % 10;
			mC[i] = 0; 
		}	
}

// Funcion para imprimir matriz
void impMatrix(int sz, double *matriz){

// Verificar si el tamaño de la matriz es menor a 12 para decidir si se imprime. ESto sirve para las pruebas para que con estos datos grandes que se manejan solo imprima el tiempo de ejecucion
	if(sz < 12){
	// Iterar sobre todos los elementos de la matriz e imprimirlos
    		for(int i = 0; i < sz*sz; i++){
    		// Salto de linea al final de cada fila
     				if(i%sz==0) printf("\n");
            		printf(" %.3f ", matriz[i]);
			}	
    	printf("\n>-------------------->\n");
	}
}

// FUncion para multiplicar matrices
void *multiMatrix(void *variables){

	// Obtener los parámetros pasados a la funcion gracias a la estructura parametros definida en la plantilla
	struct parametros *data = (struct parametros *)variables;
	
	// Obtener el ID del hilo, el número de hilos y el tamaño de la matriz
	
	int idH = data->idH;
	int nH  = data->nH;
	int N   = data->N;
	// Calcular el rango de filas que procesara el hilo
	int ini = (N/nH)*idH;
	int fin = (N/nH)*(idH+1);

	// SE iterara bajo el rango definido antes
    for (int i = ini; i < fin; i++){
        for (int j = 0; j < N; j++){
        // Se declaran los punteros auxiliares y variable para la suma
			double *pA, *pB, sumaTemp = 0.0;
			pA = mA + (i*N); 
			pB = mB + j;
			// Se suma segun la posicion respectiva
            for (int k = 0; k < N; k++, pA++, pB+=N){
				sumaTemp += (*pA * *pB);
			}
			// El resultado se almacena en la respectiva posicion de la matriz resultado
			mC[i*N+j] = sumaTemp;
		}
	}
	
	// Usar el mutex para sincronizacion (bloquear y desbloquear)
	pthread_mutex_lock (&MM_mutex);
	pthread_mutex_unlock (&MM_mutex);
	
	//Finalizar el hilo
	pthread_exit(NULL);
}

