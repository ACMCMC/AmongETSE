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
    if (strlen(E.descripcionTarea) > 0)
    {
        printf("Última tarea realizada: |%s| en |%s|\n", E.descripcionTarea, E.lugarTarea);
    }
    else
    {
        printf("Ninguna tarea asignada\n");
    }
}

void _auxContador(abb nodo, int *c)
{
    if (!es_vacio(nodo))
    {
        (*c)++;
        _auxContador(izq(nodo), c);
        _auxContador(der(nodo), c);
    }
}

int _numeroNodos(abb A)
{
    int contador = 0;
    _auxContador(A, &contador);
    return contador;
}

// Función que recorre todos los jugadores e imprime aquellos cuya habitación coincida con la buscada
void _imprimirPorHabitacion(abb A, char *habitacion)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _imprimirPorHabitacion(izq(A), habitacion);
        leer(A, &jugador);
        if (strcmp(jugador.lugarTarea, habitacion) == 0)
        {
            printf("%s: Tarea |%s| en |%s|\n", jugador.nombreJugador, jugador.descripcionTarea, jugador.lugarTarea);
        }
        _imprimirPorHabitacion(der(A), habitacion);
    }
}

void _auxBuscarPorIndice(abb A, int indiceBuscado, int *indiceActual, tipoelem *resultado)
{
    if (!es_vacio(A) && (*indiceActual <= indiceBuscado))
    {
        _auxBuscarPorIndice(izq(A), indiceBuscado, indiceActual, resultado);
        (*indiceActual)++;
        if (indiceBuscado == *indiceActual)
        {
            leer(A, resultado);
        }
        _auxBuscarPorIndice(der(A), indiceBuscado, indiceActual, resultado);
    }
}

void _buscarPorIndice(abb A, int indiceBuscado, tipoelem *resultado)
{
    int *indiceActual = (int *)malloc(sizeof(int));
    *indiceActual = 0;
    _auxBuscarPorIndice(A, indiceBuscado, indiceActual, resultado);
    free(indiceActual);
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
        printf("Nombre de Jugador (@xxx): ");
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
        printf("Nombre de Jugador (@xxx): ");
        scanf(" %s", registro.nombreJugador);
    }
    if (es_miembro(*Arbol, registro) == 0)
    {
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
        printf("%s\n", jugador.nombreJugador);
        listadoJugadores(der(A));
    }
}

//Función que genera los datos de una partida: jugadores, roles y tareas
void generarPartida(abb *Arbol)
{
    int numJugadores;
    int contador;
    char opcion;
    tipoelem jugador;
    abb arbolJuego;
    crear(&arbolJuego);
    printf("Introduce el número de jugadores: ");
    scanf(" %d", &numJugadores);
    opcion = '\0';
    while (opcion != 's' && opcion != 'S' && opcion != 'n' && opcion != 'N')
    {
        printf("Quieres hacer automáticamente el reparto de jugadores? (s/n): ");
        scanf(" %c", &opcion);
    }
    if (opcion == 's' || opcion == 'S')
    { // Repartir los jugadores automáticamente
        contador = 0;
        while (contador < numJugadores)
        {
            _buscarPorIndice(*Arbol, _aleatorio(1, _numeroNodos(*Arbol)), &jugador);
            if (!es_miembro(arbolJuego, jugador))
            {
                insertar(&arbolJuego, jugador);
                contador++;
                printf("Nombre del jugador %d: %s\n", contador, jugador.nombreJugador);
            }
        }
    }
    else
    { // Repartir los jugadores manualmente
        contador = 0;
        while (contador < numJugadores)
        {
        }
    }

    destruir(&arbolJuego);
}

//Función que imprime los datos de un usuario cuyo nombre se introduce por teclado
void consultarJugador(abb Arbol)
{
    tipoelem registro;
    *(registro.nombreJugador) = "";
    while (registro.nombreJugador[0] != '@')
    {
        printf("Nombre de Jugador (@xxx): ");
        scanf(" %s", registro.nombreJugador);
    }
    if (es_miembro(Arbol, registro))
    {
        leer(Arbol, &registro);
        _imprimirJugador(registro);
    }
    else
    {
        printf("El jugador no existe.\n");
    }
}

//Función que imprime todos los usuarios que están en una habitación determinada
void consultarPorHabitacion(abb Arbol)
{
    int numHabitacion;
    char **habitaciones = (char **)malloc(sizeof(char **) * 9);
    habitaciones[0] = "Armería";
    habitaciones[1] = "Cafetería";
    habitaciones[2] = "Comunicaciones";
    habitaciones[3] = "Electricidad";
    habitaciones[4] = "Escudos";
    habitaciones[5] = "Motor";
    habitaciones[6] = "Navegación";
    habitaciones[7] = "O2";
    habitaciones[8] = "Seguridad";
    for (numHabitacion = 0; numHabitacion < 9; numHabitacion++)
    {
        printf("\t%d. %s\n", numHabitacion + 1, habitaciones[numHabitacion]);
    }
    numHabitacion = 0;
    while (!(numHabitacion >= 1 && numHabitacion <= 9))
    {
        printf("Selecciona ubicacion: ");
        scanf("%d", &numHabitacion);
    }
    _imprimirPorHabitacion(Arbol, habitaciones[numHabitacion]);
}
