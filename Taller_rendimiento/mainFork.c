#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "mmClasicaFork.h"


int main(int argc, char *argv[]) {

	// se verifica que se ingrese la cantidad necesaria de argumentos
	if (argc < 3) {
		printf("\n \t\tUse: $./nom_ejecutable Size Hilos \n");
		exit(0);
	}
	
	// SE guarda en variables la informacion que se paso como argumento
	int N      = (int) atoi(argv[1]);
	int num_P  = (int) atoi(argv[2]);
	
	//Reservacion de memoria de las 2 matrices a multiplicar y de la que guardara el resultado
    double *matA = (double *) calloc(N*N, sizeof(double));
    double *matB = (double *) calloc(N*N, sizeof(double));
    double *matC = (double *) calloc(N*N, sizeof(double));

    srand(time(0)); // Seed for random number generation
    
    // Inicializar la matriz a partir del tamano NxN
    iniMatrix(matA, matB, N);
    
    // IMprimir las matrices generadas
    impMatrix(matA, N);
    impMatrix(matB, N);
    
    //Numero de filas por proceso
    
    int rows_per_process = N/ num_P;

	// Creacion de procesos hijo, inicio de calculo de tiempo tomado para la multiplicacion
	InicioMuestra();
    for (int i = 0; i < num_P; i++) {
        pid_t pid = fork();
        
        if (pid == 0) { // Child process
        
        // Se calcula las filas y la fila final que debe calcular este proceso hijo
            int start_row = i * rows_per_process;
            int end_row = (i == num_P - 1) ? N : start_row + rows_per_process;
            //Se multiplican las matrices en las filas previamente calculadas en el proceso
			multiMatrix(matA, matB, matC, N, start_row, end_row); 
            
            
			if(N<9){
            	// Print the portion calculated by this child
           		printf("\nChild PID %d calculated rows %d to %d:\n", getpid(), start_row, end_row-1);
            	for (int r = start_row; r < end_row; r++) {
                	for (int c = 0; c < N; c++) {
                    	printf(" %f ", matC[N*r+c]);
                	}
                	printf("\n");
            	}
			}
            exit(0); // Child exits after completing its task
            
            // Verificacion de que el proceso se haya creado bien
        } else if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
    }
    
    // Parent waits for all children to complete
    for (int i = 0; i < num_P; i++) {
        wait(NULL);
    }
  	// Para de calcular el tiempo
	FinMuestra(); 
 
 	// Liberar memoria
	free(matA);
	free(matB);
	free(matC);
	

    return 0;
}
