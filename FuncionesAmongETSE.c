#include <stdio.h>
#include <stdlib.h> //necesaria para rand()
#include <string.h> //para comparar cadenas
#include <math.h>   //para calcular el número de impostores con round()

#include "abb.h"
#include "FuncionesAmongETSE.h"

//Función privada que genera un número aleatorio entre inf y sup
unsigned int _aleatorio(int inf, int sup)
{
    return (rand() % (sup - inf + 1)) + inf;
}
//Función privada para inicializar los datos de un jugador, necesaria en varias funciones públicas
void _inicializarJugador(tipoelem *registro)
{ //inicializa los campos rol,descripcionTarea y lugarTarea
    registro->rol = ' ';
    *(registro->descripcionTarea) = '\0';
    *(registro->lugarTarea) = '\0';
}
//Función privada que imprime los datos de un jugador
void _imprimirJugador(tipoelem E)
{
    printf("Nombre jugador: %s\nDescripción tarea: %s\n", E.nombreJugador, E.descripcionTarea);
}

//Función para leer el archivo de disco
void leerArchivo(abb *A)
{
    tipoelem registro;
    FILE *fp;
    fp = fopen("ETSErsG2.txt", "rt");
    if (fp)
    {
        fscanf(fp, " %s", registro.nombreJugador);
        while (!feof(fp))
        {
            _inicializarJugador(&registro);
            insertar(A, registro);
            fscanf(fp, " %s", registro.nombreJugador);
        }
        fclose(fp);
        listadoJugadores(*A);
    }
}

//Función que añade un jugador al árbol
void altaJugador(abb *Arbol)
{
    tipoelem registro;
    *(registro.nombreJugador) = "";
    while (registro.nombreJugador[0] != '@')
    {
        printf("Introduce el nombre del jugador (empezando por @): ");
        scanf(" %s", registro.nombreJugador);
    }
    _inicializarJugador(&registro);
    insertar(Arbol, registro);
    printf("Jugador dado de alta.\n");

    // No hace falta liberar memoria, ya que todo es local
}

//Función que elimina un jugador del árbol
void bajaJugador(abb *Arbol)
{
    tipoelem registro;
    *(registro.nombreJugador) = "";
    while (registro.nombreJugador[0] != '@')
    {
        printf("Introduce el nombre del jugador (empezando por @): ");
        scanf(" %s", registro.nombreJugador);
    }
    if (es_miembro == 0) {
        printf("El jugador no es un miembro del árbol.\n");
        return;
    }
    suprimir(Arbol, registro);
    printf("Jugador dado de baja.\n");
}

//Función pública que imprime los nombres de los jugadores por orden alfabético
void listadoJugadores(abb A)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        listadoJugadores(izq(A));
        leer(A, &jugador);
        printf("\t%s\n", jugador.nombreJugador);
        listadoJugadores(der(A));
    }
}

//Función que genera los datos de una partida: jugadores, roles y tareas
void generarPartida(abb *Arbol)
{
    int numJugadores;
    char opcion;
        printf("Introduce el número de jugadores: ");
        scanf(" %d", &numJugadores);
        opcion = '\0';
        while (opcion != 's' && opcion != 'S' && opcion != 'n' && opcion != 'N') {
        printf("Quieres hacer automáticamente el reparto de jugadores? (s/n): ");
        scanf(" %c", &opcion);
        }
        if (opcion == 's' || opcion == 'S') { // Repartir los jugadores automáticamente
            _aleatorio(1, numJugadores);
        } else { // Repartir los jugadores manualmente

        }
}

//Función que imprime los datos de un usuario cuyo nombre se introduce por teclado
void consultarJugador(abb Arbol)
{
    tipoelem registro;
    *(registro.nombreJugador) = "";
    while (registro.nombreJugador[0] != '@')
    {
        printf("Introduce el nombre del jugador (empezando por @): ");
        scanf(" %s", registro.nombreJugador);
    }
    if (es_miembro(Arbol, registro)) {
        leer(Arbol, &registro);
        _imprimirJugador(registro);
    } else {
        printf("El jugador no existe.\n");
    }
}

//Función que imprime todos los usuarios que están en una habitación determinada
void consultarPorHabitacion(abb Arbol)
{
}