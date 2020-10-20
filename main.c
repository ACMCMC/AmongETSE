
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
        printf("\nBienvenid@ a AmongETSE\n\ta. Alta de jugador@\n\tb. Baja de jugador@\n\tl. Listado de jugador@s por orden alfabético\n\tg. Generar datos partida\n\tu. Consulta por jugador@ de la última tarea asignada\n\th. Consulta jugador@s por habitación\n\ts. Salir de la aplicación\n");
    while(opcion != 's' && opcion != 'S') {
        printf("\nOpcion: ");
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

