
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
        printf("MENÚ\n\ta) Dar un jugador de alta\n\tb) Dar un jugador de baja\n\tl) Imprimir un listado de jugadores\n\tg) Generar una partida\n\tu) Consultar la última tarea asignada a un jugador\n\th) Consultar los jugadores con última tarea en una habitación\n");
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
            generarPartida(&arbolJugadores);
            break;
            case 'u':
            case 'U':
            consultarJugador(arbolJugadores);
            break;
            case 'h':
            case 'H':
            consultarPorHabitacion(arbolJugadores);
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

