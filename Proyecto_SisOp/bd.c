#include "bd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define FORMATO_FECHA "%02d-%02d-%04d"

//variables globales para la BD
Libro baseDatos[MAX_LIBROS];
int   totalLibros = 0;

//obtiene la fecha actual del sistema y la escribe en fecha_out formato "dd-mm-aaaa".
static void obtener_fecha_actual(char *fecha_out) {
    time_t t = time(NULL);
    struct tm tm_actual = *localtime(&t);
    snprintf(fecha_out, 36, FORMATO_FECHA,
             tm_actual.tm_mday,
             tm_actual.tm_mon + 1,
             tm_actual.tm_year + 1900);
}

int cargarBD(const char *archivo) {
    FILE *f = fopen(archivo, "r");
    if (!f) {
        perror("bd.c: fopen cargarBD");
        return 0;
    }

    totalLibros = 0;
    char linea[256];

    while (fgets(linea, sizeof(linea), f)) {
        //saltar líneas vacías
        if (linea[0] == '\n' || strchr(linea, ',') == NULL)
            continue;

        //variables para leer la cabecera del libro
        char nombreTemp[100];
        int  isbnTemp;
        int  cantidadTemp;

        //leer nombre, ISBN, cantidad"
        if (sscanf(linea, " %99[^,] , %d , %d",
                   nombreTemp, &isbnTemp, &cantidadTemp) != 3) {
            //linea invalida: ignorar
            continue;
        }

        //inicializar estructura libro
        Libro lib;
        strncpy(lib.nombre, nombreTemp, sizeof(lib.nombre));
        lib.nombre[sizeof(lib.nombre) - 1] = '\0';
        lib.isbn     = isbnTemp;
        lib.cantidad = cantidadTemp;

        //leer ejemplares
        for (int i = 0; i < cantidadTemp; i++) {
            if (!fgets(linea, sizeof(linea), f))
                break;  // si no hay mas líneas, sale

            int  numEjemplar;
            char estadoEj;
            char fechaTemp[20];

            if (sscanf(linea, " %d , %c , %19[^\n]",
                       &numEjemplar, &estadoEj, fechaTemp) != 3) {
                //si linea de ejemplar es invalida asume estado 'D' sin fecha
                numEjemplar = i + 1;
                estadoEj    = 'D';
                strcpy(fechaTemp, "01-01-1970");
            }

            lib.ejemplares[i].numero = numEjemplar;
            lib.ejemplares[i].estado = estadoEj;
            strncpy(lib.ejemplares[i].fecha, fechaTemp,
                    sizeof(lib.ejemplares[i].fecha));
            lib.ejemplares[i].fecha[sizeof(lib.ejemplares[i].fecha)-1] = '\0';
        }

        //guardar en el arreglo global
        if (totalLibros < MAX_LIBROS) {
            baseDatos[totalLibros++] = lib;
        } else {
            fprintf(stderr, "bd.c: cantidad de libros excede MAX_LIBROS\n");
            break;
        }
    }

    fclose(f);
    return 1;
}

Libro *buscarLibro(int isbn) {
    for (int i = 0; i < totalLibros; i++) {
        if (baseDatos[i].isbn == isbn) {
            return &baseDatos[i];
        }
    }
    return NULL;
}

int prestarEjemplar(Libro *libro) {
    for (int i = 0; i < libro->cantidad; i++) {
        if (libro->ejemplares[i].estado == 'D') {
            libro->ejemplares[i].estado = 'P';
            obtener_fecha_actual(libro->ejemplares[i].fecha);
            return libro->ejemplares[i].numero;
        }
    }
    return -1; //no hay ejemplares disponibles
}

int devolverEjemplar(Libro *libro) {
    for (int i = 0; i < libro->cantidad; i++) {
        if (libro->ejemplares[i].estado == 'P') {
            libro->ejemplares[i].estado = 'D';
            obtener_fecha_actual(libro->ejemplares[i].fecha);
            return libro->ejemplares[i].numero;
        }
    }
    return -1; //no hay ejemplares prestados
}

int renovarEjemplar(Libro *libro) {
    for (int i = 0; i < libro->cantidad; i++) {
        if (libro->ejemplares[i].estado == 'P') {
            //al renovar actualizamos a la fecha de hoy
            obtener_fecha_actual(libro->ejemplares[i].fecha);
            return libro->ejemplares[i].numero;
        }
    }
    return -1; //no hay ejemplares prestados para renovar
}

void imprimirEstado(void) {
    for (int i = 0; i < totalLibros; i++) {
        Libro *l = &baseDatos[i];
        for (int j = 0; j < l->cantidad; j++) {
            printf("%c, %s, %d, %d, %s\n",
                   l->ejemplares[j].estado,
                   l->nombre,
                   l->isbn,
                   l->ejemplares[j].numero,
                   l->ejemplares[j].fecha);
        }
    }
}

void guardarEstado(const char *archivo) {
    FILE *f = fopen(archivo, "w");
    if (!f) {
        perror("bd.c: fopen guardarEstado");
        return;
    }

    for (int i = 0; i < totalLibros; i++) {
        Libro *l = &baseDatos[i];
        //primera línea con "Nombre, ISBN, Cantidad"
        fprintf(f, "%s, %d, %d\n",
                l->nombre,
                l->isbn,
                l->cantidad);
        //una línea por cada ejemplar
        for (int j = 0; j < l->cantidad; j++) {
            fprintf(f, "%d, %c, %s\n",
                    l->ejemplares[j].numero,
                    l->ejemplares[j].estado,
                    l->ejemplares[j].fecha);
        }
    }

    fclose(f);
}

