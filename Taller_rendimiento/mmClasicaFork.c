#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include "mmClasicaFork.h"

//Variables globales
struct timeval inicio, fin;

// FUncion que inicia medicion de tiempo
void InicioMuestra(){

	// Obtener el tiempo actual y almacenar
	gettimeofday(&inicio, (void *)0);
}

// Funcion que termina la medicion de tiempo
void FinMuestra(){
	gettimeofday(&fin, (void *)0);
	
	// Calcular la diferencia entre el primer tiempo y el segundo
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double) (fin.tv_sec*1000000 + fin.tv_usec);
	//IMprimir resultado
	printf("%9.0f \n", tiempo);
}

// Funcion para multiplicar la matriz a partir de las filas dadas
void multiMatrix(double *mA, double *mB, double *mC, int D, int filaI, int filaF) {

	// Varaibles en donde se guardaran las sumas y las posiciones de cada uno de las 2 matrices
	double Suma, *pA, *pB;
	
	// SE itera sobre las filas dadas
    for (int i = filaI; i < filaF; i++) {
        for (int j = 0; j < D; j++) {
        
        	// Se inicializa las sumas y los punteros
			Suma = 0;
			pA = mA+i*D;
			pB = mB+j;
			
			// Se suma segun la posicion respectiva
            for (int k = 0; k < D; k++, pA++, pB+=D) {
				Suma += *pA * *pB;	
            }
            	// El resultado se almacena en la respectiva posicion de la matriz resultado
			mC[i*D+j] = Suma;
        }
    }
}

// IMpresion de matriz 
void impMatrix(double *matrix, int D) {

	// Verificar si el tamaño de la matriz es menor a 9 para decidir si se imprime. ESto sirve para las pruebas para que con estos datos grandes que se manejan solo imprima el tiempo de ejecucion
	if (D < 9) {
    	printf("\nImpresión	...\n");
    	
    	// Iterar sobre todos los elementos de la matriz e imprimirlos
    	for (int i = 0; i < D*D; i++, matrix++) {
    			// Salto de linea al final de cada fila
			if(i%D==0) printf("\n");
            	printf(" %f ", *matrix);
        	}
        printf("\n ");
    }
}

// Function to initialize matrix with random values
void iniMatrix(double *mA, double *mB, int D){
	for (int i = 0; i < D*D; i++, mA++, mB++){
	
	// En cada posicion, se saca un numero aleatorio hasta el 10 y se pone en las 2 matrices
            *mA = rand() % 10; 
            *mB = rand() % 10; 
        }
}

