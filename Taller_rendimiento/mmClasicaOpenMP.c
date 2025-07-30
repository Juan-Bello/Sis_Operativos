#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include "mmClasicaOpenMP.h"

//Variables globales
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


// Funcion para imprimir la matriz
void impMatrix(size_t *matrix, int D){
	printf("\n");
	
	// Verificar si el tamaño de la matriz es menor a 9 para decidir si se imprime. ESto sirve para las pruebas para que con estos datos grandes que se manejan solo imprima el tiempo de ejecucion
	if(D < 9){
	// Iterar sobre todos los elementos de la matriz e imprimirlos
		for(int i=0; i<D*D; i++){
		// Salto de linea al final de cada fila
			if(i%D==0) printf("\n");
			printf("%zu ", matrix[i]);
		}
		printf("\n**-----------------------------**\n");
	}
}


// FUncion para inicializar la matriz
void iniMatrix(size_t *m1, size_t *m2, int D){
	for(int i=0; i<D*D; i++, m1++, m2++){
	// Se multiplica y suma el i actual por dos y el resultado se guarda en las matrices
		*m1 = i*2; // (size_t) rand()%10;	
		*m2 = i+2; // (size_t) rand()%10;	
	}
}

// Funcion para multiplicar las matrices
void multiMatrix(size_t *mA, size_t *mB, size_t *mC, int D){
	// Declarar variables para la suma y punteros para recorres las matrices
	size_t Suma, *pA, *pB;
	
	//Crea varios hilos, en donde se ejecuta el codigo en paralelo
	#pragma omp parallel
	{
	// Distribuir las iteraciones del bucle entre los hilos creados
	#pragma omp for
	for(int i=0; i<D; i++){
		for(int j=0; j<D; j++){
			// Inicializar las variables punteros a la fila i de la matriz A y a la columna j de la matriz B
			pA = mA+i*D;	
			pB = mB+j; //pB = mB+(j*D);	
			Suma = 0.0;
			
			// Iterar para calcular la suma
			for(int k=0; k<D; k++, pA++, pB+=D){
			
				// Sumar la respectiva multiplicacion de cada uno
				Suma += *pA * *pB;
			}
			
			// Guardar el resultado en una posicion en especifico de la matriz c
			mC[i*D+j] = Suma;
		}
	}
	}
}

