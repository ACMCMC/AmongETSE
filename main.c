
#include <stdio.h>
#include <stdlib.h>
#include <time.h> //para utilizar la semilla aleatoria time(NULL)
#include "FuncionesAmongETSE.h"


int main(int argc, char** argv) {
    char opcion;
    
    srand((unsigned int) time(NULL)); //semilla para aleatorios, se llamar sólo una vez al principio de main
    
    //Crear el árbol de jugadores
    abb arbolJugadores;

    //Leer el archivo de disco
    leerArchivo(&arbolJugadores);

opcion = '\0';
    //Menú
    while(opcion != 's') {
        printf("Introduce tu opción: ");
        scanf(" %c", &opcion);
        switch (opcion) {
            case 'a':
            case 'A':
            altaJugador(&arbolJugadores);
            break;
            case 'b':
            case 'B':
            bajaJugador(&arbolJugadores);
            break;
            case 'l':
            case 'L':
            listadoJugadores(arbolJugadores);
            break;
            case 'g':
            case 'G':
            break;
            case 'u':
            case 'U':
            break;
            case 'h':
            case 'H':
            break;
            case 's':
            case 'S':
            printf("Saliendo del juego...\n");
            break;
            default:
            printf("Opción incorrecta.\n");
        }
    }
    
    //Destruir el árbol al finalizar

    return (EXIT_SUCCESS);
}

