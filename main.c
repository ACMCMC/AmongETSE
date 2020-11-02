/*
Modificaciones con respecto a la especificación:

- Un impostor solo mata a un tripulante si en la habitación en la que está hay más de un tripulante (ya que si solo estuvieran el impostor y un tripulante, se habría delatado).
- El archivo con los nombres de los jugadores es jugadores.txt
- Los impostores solo matan a otros jugadores el 66% de las veces
- "Consultar por habitación" devuelve todos los jugadores que tengan al menos una de sus tareas en esa habitación
- Al matar a los jugadores, se recorre aleatoriamente el árbol. De esta forma, hacemos el juego más impredecible, ya que si se recorriese haciendo un recorrido estándar, sabríamos que los primeros jugadores en morir se corresponden con los primeros impostores de la lista (en el caso del inorden). Si recorremos el árbol de forma aleatoria, entonces no se puede predecir esto.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h> //para utilizar la semilla aleatoria time(NULL)
#include "FuncionesAmongETSE.h"

int main()
{
    char opcion;

    srand((unsigned int)time(NULL)); //semilla para aleatorios, se llamar sólo una vez al principio de main

    //Crear el árbol de jugadores
    abb arbolJugadores;
    crear(&arbolJugadores);

    //Leer el archivo de disco
    leerArchivo(&arbolJugadores);

    opcion = '\0';
    //Menú
    while (opcion != 's' && opcion != 'S')
    {
        printf("\nBienvenid@ a AmongETSE\n\ta. Alta de jugador@\n\tb. Baja de jugador@\n\tl. Listado de jugador@s por orden alfabético\n\tg. Jugar\n\tu. Consulta por jugador@ de la próxima tarea a realizar\n\th. Consulta jugador@s por habitación\n\tf. Guardar archivo\n\ts. Salir de la aplicación\n");
        printf("\nOpcion: ");
        scanf(" %c", &opcion);
        switch (opcion)
        {
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
            jugar(&arbolJugadores);
            break;
        case 'u':
        case 'U':
            consultarTareaJugador(arbolJugadores);
            break;
        case 'h':
        case 'H':
            consultarPorHabitacion(arbolJugadores);
            break;
        case 'f':
        case 'F':
            guardarArchivo(arbolJugadores);
            break;
        case 's':
        case 'S':
            printf("Adios\n");
            break;
        default:
            printf("Opción incorrecta.\n");
        }
    }
    //Destruir el árbol al finalizar
    destruir(&arbolJugadores);

    return (EXIT_SUCCESS);
}
