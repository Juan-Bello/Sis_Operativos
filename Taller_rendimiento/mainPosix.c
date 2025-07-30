#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "mmClasicaPosix.h"

int main(int argc, char *argv[]){

	// se verifica que se ingrese la cantidad necesaria de argumentos
	if (argc < 3){
		printf("Ingreso de argumentos \n $./ejecutable tamMatriz numHilos\n");
		exit(0);	
	}
	
	// SE guarda en variables la informacion que se paso como argumento
    int SZ = atoi(argv[1]); 
    int n_threads = atoi(argv[2]); 
	
	
	// Se crean los hilos a partir del numero ingresado por el usuario
    pthread_t p[n_threads];
    pthread_attr_t atrMM;

	// reservar memoria las matrices a partir de DATA_SIZE (1024*1024*64*3) 
	mA = MEM_CHUNK;
	mB = mA + SZ*SZ;
	mC = mB + SZ*SZ;
	
	//Inicializar las matrices a partir del tamano NxN

	iniMatrix(SZ);
	impMatrix(SZ, mA);
	impMatrix(SZ, mB);
	
	// Inicio del conteo de tiempo de ejecucion

	InicioMuestra();
	
	// Inicializar el mutex y los atributos de los hilos
	pthread_mutex_init(&MM_mutex, NULL);
	pthread_attr_init(&atrMM);
	pthread_attr_setdetachstate(&atrMM, PTHREAD_CREATE_JOINABLE);
	
	// Crear los hilos para realizar la multiplicación de matrices

    for (int j=0; j<n_threads; j++){
		struct parametros *datos = (struct parametros *) malloc(sizeof(struct parametros)); 
		datos->idH = j;
		datos->nH  = n_threads;
		datos->N   = SZ;
        pthread_create(&p[j],&atrMM,multiMatrix,(void *)datos);
	}
//ESperar a que los hilos terminen
    for (int j=0; j<n_threads; j++)
        pthread_join(p[j],NULL);
        
        
	//Finalizar conteo
	FinMuestra();
	
	
	// Imprimir el resultado
	impMatrix(SZ, mC);

	//Liberacion de recursos y el mutex
	pthread_attr_destroy(&atrMM);
	pthread_mutex_destroy(&MM_mutex);
	
	
	pthread_exit (NULL);
	
	return 0;
}

