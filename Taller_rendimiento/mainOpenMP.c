#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#include "mmClasicaOpenMP.h"
int main(int argc, char *argv[]){

	// se verifica que se ingrese la cantidad necesaria de argumentos
	if(argc < 3){
		printf("\n Use: $./clasicaOpenMP SIZE Hilos \n\n");
		exit(0);
	}

	// SE guarda en variables la informacion que se paso como argumento
	int N = atoi(argv[1]);
	int TH = atoi(argv[2]);
	
	//Reservacion de memoria de las 2 matrices a multiplicar y de la que guardara el resultado
	size_t *matrixA  = (size_t *)calloc(N*N, sizeof(size_t));
	size_t *matrixB  = (size_t *)calloc(N*N, sizeof(size_t));
	size_t *matrixC  = (size_t *)calloc(N*N, sizeof(size_t));
	
	// Nos servira para generar los numeros aleatorios
	srand(time(NULL));
	
	
	// Metodo para definir el numero de hilos a usar, se pasa de parametro TH, que tiene el numero de hilos que el usuario paso de argumento
	omp_set_num_threads(TH);

	//INicializar las matrices de tamano NxN
	iniMatrix(matrixA, matrixB, N);

	// IMprimir las matrices una vez inicializadas
	impMatrix(matrixA, N);
	impMatrix(matrixB, N);


	// Inicio del conteo de tiempo de ejecucion y multiplicacion de la matriz
	InicioMuestra();
	multiMatrix(matrixA, matrixB, matrixC, N);
	
	// Fin del conteo de tiempo
	FinMuestra();
	
	// IMpresion del resultado de la multiplicacion

	impMatrix(matrixC, N);

	/*Liberación de Memoria*/
	free(matrixA);
	free(matrixB);
	free(matrixC);
	
	return 0;
}
