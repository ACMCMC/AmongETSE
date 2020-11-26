#include <stdio.h>
#include <stdlib.h>
#include <time.h> //para utilizar la semilla aleatoria time(NULL)

#include "FuncionesAEMaps.h"
#include "FuncionesAmongETSE.h"
#include "ConstantesAmongETSE.h"
#include "abb.h"
#include "grafo.h"

/*
Modificaciones con respecto a la especificación:

- Un impostor solo mata a un tripulante si en la habitación en la que está hay más de un tripulante (ya que si solo estuvieran el impostor y un tripulante, se habría delatado).
- El archivo con los nombres de los jugadores es jugadores.txt
- Los impostores solo matan a otros jugadores el 66% de las veces
- "Consultar por habitación" devuelve todos los jugadores que tengan al menos una de sus tareas en esa habitación
- Al matar a los jugadores, se recorre aleatoriamente el árbol. De esta forma, hacemos el juego más impredecible, ya que si se recorriese haciendo un recorrido estándar, sabríamos que los primeros jugadores en morir se corresponden con los primeros impostores de la lista (en el caso del inorden). Si recorremos el árbol de forma aleatoria, entonces no se puede predecir esto.

*/

int main(int argc, char **argv)
{
    grafo G; // grafo
    abb arbolJugadores; // árbol de jugadores
    char opcion;

    srand((unsigned int)time(NULL)); //semilla para aleatorios, se llamar sólo una vez al principio de main

    //Crear el árbol de jugadores
    crear(&arbolJugadores);
    //Leer el archivo de jugadores desde disco
    leerArchivoJugadores(&arbolJugadores);

    //Creo el grafo
    crear_grafo(&G);
    leerArchivoGrafo(&G);
    imprimir_grafo(G);
    listadoJugadores(arbolJugadores);
    do
    {
        // Imprimimos un menú básico
        printf("\n\nBienvenid@ a AmongETSE\n\n");
        printf("\ta. Alta de jugador@\n");
        printf("\tb. Baja de jugador@\n");
        printf("\tc. Crear comunicacion\n");
        printf("\td. Eliminar comunicacion\n");
        printf("\tf. Guardar archivos\n");
        printf("\tg. Jugar\n");
        printf("\th. Anadir habitacion\n");
        printf("\ti. Imprimir mapa\n");
        printf("\tl. Listado de jugador@s por orden alfabético\n");
        printf("\tq. Clausurar habitacion\n");
        printf("\ts. Salir\n");

        printf("Opcion: ");
        scanf(" %c", &opcion); // Leemos la opción del usuario

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
        case 'h':
        case 'H':
            introducir_vertice(&G);
            break;
        case 'q':
        case 'Q':
            eliminar_vertice(&G);
            break;
        case 'c':
        case 'C':
            nuevo_arco(&G);
            break;
        case 'd':
        case 'D':
            eliminar_arco(&G);
            break;
        case 'i':
        case 'I':
            imprimir_grafo(G);
            break;
        case 'l':
        case 'L':
            listadoJugadores(arbolJugadores);
            break;
        case 'f':
        case 'F':
            guardarArchivoGrafo(G);
            guardarArchivoJugadores(arbolJugadores);
            break;
        case 'g':
        case 'G':
            jugar(&arbolJugadores, G);
            break;
        case 's':
        case 'S':
            opcion = 's';
            printf("Adios\n");
            break;
        default:
            printf("Opción equivocada\n");
        }
    } while (opcion != 's');

    //Al salir, liberamos la memoria de los TAD, los destruimos
    borrar_grafo(&G);
    destruir(&arbolJugadores);

    return (EXIT_SUCCESS);
}
