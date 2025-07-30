#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "bd.h"     // define Libro, Ejemplar y funciones de BD

#define MAX_BUFFER 10

//estructura de solicitud
typedef struct {
    char operacion;      // 'P', 'D' o 'R'
    char nombre[100];    // nombre del libro 
    int isbn;            // ISBN para buscar en la BD
} Solicitud;

//buffer circular
static Solicitud buffer[MAX_BUFFER];
static int in = 0, out = 0, count = 0;

//sincronizacion
static pthread_mutex_t mutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t lleno    = PTHREAD_COND_INITIALIZER;
static pthread_cond_t vacio    = PTHREAD_COND_INITIALIZER;

//flags globales
static bool  terminar    = false;
static int   verbose     = 0;
static char  archivo_salida[256] = "";
static int   fd_pipe     = -1;

//hilo 1: devoluciones (D) y renovaciones (R)
void *hilo_aux1_func(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        // espera hasta que haya algo en el buffer o se pida terminar
        while (count == 0 && !terminar)
            pthread_cond_wait(&vacio, &mutex);

        // si pide terminar y no quedan solicitudes se sale
        if (terminar && count == 0) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // tomar solicitud del buffer
        Solicitud s = buffer[out];
        out = (out + 1) % MAX_BUFFER;
        count--;
        pthread_cond_signal(&lleno);
        pthread_mutex_unlock(&mutex);

        // buscar libro en BD
        Libro *libro = buscarLibro(s.isbn);
        if (!libro) {
            printf("[aux1] Libro no encontrado (ISBN %d)\n", s.isbn);
            continue;
        }

        // procesar segun operacion
        if (s.operacion == 'D') {
            int ej = devolverEjemplar(libro);
            if (ej == -1)
                printf("[aux1] No se pudo devolver ejemplar de %s\n", libro->nombre);
            else
                printf("[aux1] Libro devuelto: %s (ej %d)\n", libro->nombre, ej);

        } else if (s.operacion == 'R') {
            int ej = renovarEjemplar(libro);
            if (ej == -1)
                printf("[aux1] No se pudo renovar ejemplar de %s\n", libro->nombre);
            else
                printf("[aux1] Libro renovado: %s (ej %d)\n", libro->nombre, ej);
        }
    }
    return NULL;
}

//hilo 2: escucha comandos de consola
void *hilo_aux2_func(void *arg) {
    while (!terminar) {
        char cmd;
        printf("\n[RP] Comando (r=Reporte, s=Salir): \n");
        if (scanf(" %c", &cmd) != 1) break;

        if (cmd == 'r') {
            printf("\n=== REPORTE ACTUAL ===\n");
            imprimirEstado();
        }
        else if (cmd == 's') {
            printf("[RP] Finalizando ejecución...\n");
            if (archivo_salida[0]) {
                printf("[RP] Guardando estado en %s\n", archivo_salida);
                guardarEstado(archivo_salida);
            }
            // señalar cierre
            terminar = true;
            // cerrar pipe 
            if (fd_pipe != -1) close(fd_pipe);
            // despertar hilo_aux1 si esta esperando
            pthread_cond_signal(&vacio);
            break;
        }
        else {
            printf("[RP] Comando inválido.\n");
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    char pipe_name[256] = "";
    char archivo_bd[256] = "";

    //deteccion de flags
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i+1 < argc) {
            strcpy(pipe_name, argv[++i]);
        }
        else if (strcmp(argv[i], "-f") == 0 && i+1 < argc) {
            strcpy(archivo_bd, argv[++i]);
        }
        else if (strcmp(argv[i], "-s") == 0 && i+1 < argc) {
            strcpy(archivo_salida, argv[++i]);
        }
        else if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        }
    }
    if (!pipe_name[0] || !archivo_bd[0]) {
        fprintf(stderr, "Uso: %s -p pipe -f archivoBD [-s archivoSalida] [-v]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //cargar BD
    if (!cargarBD(archivo_bd)) {
        fprintf(stderr, "Error cargando BD (%s)\n", archivo_bd);
        exit(EXIT_FAILURE);
    }

    //crear pipe FIFO si no existe
    mkfifo(pipe_name, 0666);

    //iniciar hilos
    pthread_t hilo1, hilo2;
    pthread_create(&hilo1, NULL, hilo_aux1_func, NULL);
    pthread_create(&hilo2, NULL, hilo_aux2_func, NULL);

    //abrir pipe para lectura
    fd_pipe = open(pipe_name, O_RDONLY);
    if (fd_pipe == -1) {
        perror("receptor: open pipe");
        exit(EXIT_FAILURE);
    }

    //fdopen/fgets para leer línea a línea
    FILE *fp = fdopen(fd_pipe, "r");
    if (!fp) {
        perror("receptor: fdopen");
        close(fd_pipe);
        exit(EXIT_FAILURE);
    }

    //bucle recibir solicitudes
    char linea[256];
    while (!terminar && fgets(linea, sizeof(linea), fp)) {
        Solicitud s;
        if (sscanf(linea, " %c , %99[^,] , %d", 
                   &s.operacion, s.nombre, &s.isbn) < 3) {
            continue;
        }
        if (verbose) {
            printf("[VERBOSE] Solicitud: %c - %s - %d\n",
                   s.operacion, s.nombre, s.isbn);
        }
        if (s.operacion == 'D' || s.operacion == 'R') {
            // insertar en buffer circular
            pthread_mutex_lock(&mutex);
            while (count == MAX_BUFFER && !terminar)
                pthread_cond_wait(&lleno, &mutex);
            if (terminar) {
                pthread_mutex_unlock(&mutex);
                break;
            }
            buffer[in] = s;
            in = (in + 1) % MAX_BUFFER;
            count++;
            pthread_cond_signal(&vacio);
            pthread_mutex_unlock(&mutex);
        }
        else if (s.operacion == 'P') {
            //prestamo directo
            Libro *lib = buscarLibro(s.isbn);
            if (!lib) {
                printf("[RP] Libro no encontrado (ISBN %d)\n", s.isbn);
            } else {
                int ej = prestarEjemplar(lib);
                if (ej == -1)
                    printf("[RP] Sin ejemplares disponibles de %s\n", lib->nombre);
                else
                    printf("[RP] Prestado: %s (ej %d)\n", lib->nombre, ej);
            }
        }
    }

    //cierre
    if (fp) fclose(fp);
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    return 0;
}

