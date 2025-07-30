#ifndef BD_H
#define BD_H

#include <stdio.h>

#define MAX_EJEMPLARES 20
#define MAX_LIBROS     100

//estructura de un ejemplar
typedef struct {
    int  numero;          // identificador interno del ejemplar
    char estado;          // 'P' = Prestado, 'D' = Disponible
    char fecha[20];       // fecha de la última operación (dd-mm-aaaa)
} Ejemplar;

//estructura de un libro con todos sus ejemplares
typedef struct {
    char   nombre[100];            // nombre completo del libro
    int    isbn;                   // identificador único ISBN
    int    cantidad;               // número de ejemplares totales
    Ejemplar ejemplares[MAX_EJEMPLARES]; 
} Libro;

//base de datos global y contador de libros cargados
extern Libro baseDatos[MAX_LIBROS];
extern int   totalLibros;

//arga la base de datos desde un archivo de texto. Devuelve 1 si se cargó correctamente, 0 en caso de error.
int cargarBD(const char *archivo);

//busca un libro en la base de datos según su ISBN. Retorna puntero al Libro si se encuentra, o NULL si no existe.
Libro *buscarLibro(int isbn);

//presta el primer ejemplar disponible ('D') del libro dado. Cambia su estado a 'P' y actualiza la fecha con la fecha actual.Retorna el número de ejemplar prestado (>=0), o -1 si no hay disponibles.
int prestarEjemplar(Libro *libro);

//devuelve el primer ejemplar prestado ('P') del libro dado. Cambia su estado a 'D' y actualiza la fecha con la fecha actual.Retorna el número de ejemplar devuelto (>=0), o -1 si no hay ejemplares prestados.
int devolverEjemplar(Libro *libro);

//Renueva el primer ejemplar prestado ('P') del libro dado. Actualiza su fecha con la fecha actual. Retorna el número de ejemplar renovado o -1 si no hay ejemplares prestados.
int renovarEjemplar(Libro *libro);

//imprime el estado completo de la base de datos.
void imprimirEstado(void);

//guarda el estado actual de la base de datos en un archivo de texto.
void guardarEstado(const char *archivo);

#endif // BD_H

