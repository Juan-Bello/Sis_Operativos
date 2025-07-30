#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define DEF_PIPE "pipeRP"

void procesar_consola(int fd);
void procesar_archivo(int fd, const char *nombre_archivo);

int main(int argc, char *argv[]) {
    const char *pipe_name = DEF_PIPE;
    const char *archivo_entrada = NULL;
    int fd;

    // flags: -p <pipe>  y  -i <archivo>
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && (i + 1) < argc) {
            pipe_name = argv[++i];
        }
        else if (strcmp(argv[i], "-i") == 0 && (i + 1) < argc) {
            archivo_entrada = argv[++i];
        }
        else {
            fprintf(stderr, "Uso: %s [-p nombre_pipe] [-i archivo_solicitudes]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // abrir pipe en modo escritura
    fd = open(pipe_name, O_WRONLY);
    if (fd == -1) {
        perror("solicitante: open pipe");
        exit(EXIT_FAILURE);
    }

    if (archivo_entrada) {
        // modo por archivo
        procesar_archivo(fd, archivo_entrada);
    } else {
        // modo consola
        procesar_consola(fd);
    }

    close(fd);
    return 0;
}

// lee líneas desde stdin, una solicitud a la vez, formatea y envía al pipe.
void procesar_consola(int fd) {
    while (1) {
        char op;
        char nombre[100];
        int isbn;

        printf("\nOperación (D=Devolver, R=Renovar, P=Prestar, Q=Salir): ");
        if (scanf(" %c", &op) != 1) {
            // EOF o error de lectura
            break;
        }

        if (op == 'Q') {
            printf("Solicitante: saliendo.\n");
            break;
        }

        printf("Nombre del libro: ");
        // salta el \n previo y lee toda la línea hasta '\n'
        scanf(" %[^\n]", nombre);

        printf("ISBN: ");
        scanf("%d", &isbn);

        // construir mensaje con formato op,nombre,ISBN\n"
        char mensaje[256];
        int len = snprintf(mensaje, sizeof(mensaje), "%c,%s,%d\n",
                           op, nombre, isbn);
        if (len < 0 || len >= (int)sizeof(mensaje)) {
            fprintf(stderr, "solicitante: mensaje demasiado largo\n");
            continue;
        }

        if (write(fd, mensaje, len) != len) {
            perror("solicitante: write");
            break;
        }
        printf("Solicitud enviada: %s", mensaje);
    }
}


//lee todas las líneas del archivo de solicitudes donde cada línea debe tener el formato op(D,R o P),nombre,ISBN luego por cada línea correctamente parseada, envía "OP,NOMBRE,ISBN\n" al pipe.
void procesar_archivo(int fd, const char *nombre_archivo) {
    FILE *f = fopen(nombre_archivo, "r");
    if (!f) {
        perror("solicitante: fopen archivo de entrada");
        return;
    }

    char linea[256];
    int linea_num = 0;
    while (fgets(linea, sizeof(linea), f)) {
        linea_num++;

        // omitir líneas vacías o que comiencen con '#'
        if (linea[0] == '\n' || linea[0] == '#')
            continue;

        // leer la línea: op,nombre,ISBN
        char op;
        char nombre[100];
        int isbn;
        if (sscanf(linea, " %c , %99[^,] , %d", &op, nombre, &isbn) < 3) {
            fprintf(stderr,
                "solicitante: formato inválido en línea %d: %s",
                linea_num, linea);
            continue;
        }

        // construir mensaje para el pipe
        char mensaje[256];
        int len = snprintf(mensaje, sizeof(mensaje), "%c,%s,%d\n",
                           op, nombre, isbn);
        if (len < 0 || len >= (int)sizeof(mensaje)) {
            fprintf(stderr,
                "solicitante: línea %d demasiado larga, omitiendo\n",
                linea_num);
            continue;
        }

        // enviar al pipe
        if (write(fd, mensaje, len) != len) {
            perror("solicitante: write desde archivo");
            break;
        }
        printf("Enviado (línea %d): %s", linea_num, mensaje);
    }

    fclose(f);
    printf("Solicitante: fin de archivo %s\n", nombre_archivo);
}

