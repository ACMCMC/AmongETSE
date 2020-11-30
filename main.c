#include <stdio.h>
#include <stdlib.h>
#include <time.h> //para utilizar la semilla aleatoria time(NULL)

#include "FuncionesAEMaps.h"
#include "FuncionesAmongETSE.h"
#include "ConstantesAmongETSE.h"
#include "abb.h"
#include "grafo.h"

// Aldán Creo Mariño, AED 2020/21

/*
SOBRE LA PRÁCTICA 7b

En mi versión de AmongETSE, lo que se hace es que cada tick de tiempo los jugadores se mueven una unidad de distancia, hacia la habitación donde tienen su tarea. He añadido los siguientes campos al tipoelem (que representa al jugador):
- La siguiente habitación en la ruta hacia la habitación de la tarea
- La habitación anterior en la que estuvieron
- La distancia que falta hasta llegar a la siguiente habitación
- Una vez que llegan a la habitación de la tarea, se espera un tiempo aleatorio entre TIEMPO_MIN_TAREA y TIEMPO_MAX_TAREA (definidos en ConstantesAmongETSE.h), para representar lo que tardan en realizar esa tarea (tiempoRestanteTarea).

Mientras están en una habitación (ya sea de paso, o ejecutando una tarea), un impostor que esté en esa misma habitación, podrá matar al tripulante. El juego no se para en ese momento, sino que se para cuando otro jugador (impostor o tripulante), descubre el cadáver del tripulante muerto. Es entonces cuando se muestra un menú al jugador para que decida quién cree que es el impostor. Una vez el cadáver es encontrado, lo "retiro del mapa" (no del árbol, sino que simplemente vacío todos sus campos para que no influya en el desarrollo de la partida).

He introducido la opción de que el usuario decida si mostrar si el jugador que decide matar es tripulante o impostor (ya que así funciona en AmongUs original).

No he incluido ninguna de las funciones del AEMaps en el menú del AmongETSE, ya que no le veo mucha lógica a hacerlo, sino que simplemente reutilizo _floyd del AEMaps (adaptado para que sea últil para el AmongETSE). En cualquier caso, el usuario no puede interactuar directamente con esa función.

También he subido el número de tareas que realizan a 10 (se puede especificar en ConstantesAmongETSE.h), ya que si no a veces la partida termina muy rápido, ya que las tareas se van ejecutando automáticamente sin que intervenga el usuario.

He incrementado el máximo de jugadores a 15, porque si no las partidas a veces acababan muy rápido (los impostores tienen más facilidad para matar en esta versión de AmongETSE, ya que los tripulantes se quedan esperando en las habitaciones mientras hacen las tareas).

He tenido que adaptar bastantes otras funciones del AmongETSE, pero en general los cambios de comportamiento de cara al usuario serían esos.
*/

/*
Modificaciones con respecto a la especificación del AmongETSE (v3):

- Un impostor solo mata a un tripulante si en la habitación en la que está hay más de un tripulante (ya que si solo estuvieran el impostor y un tripulante, se habría delatado).
- El archivo con los nombres de los jugadores es jugadores.txt
- Los impostores solo matan a otros jugadores el 66% de las veces
- "Consultar por habitación" devuelve todos los jugadores que tengan al menos una de sus tareas en esa habitación
- Al matar a los jugadores, se recorre aleatoriamente el árbol. De esta forma, hacemos el juego más impredecible, ya que si se recorriese haciendo un recorrido estándar, sabríamos que los primeros jugadores en morir se corresponden con los primeros impostores de la lista (en el caso del inorden). Si recorremos el árbol de forma aleatoria, entonces no se puede predecir esto.

*/

int main(int argc, char **argv)
{
    grafo G;            // grafo
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
