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
    registro->rol = NO_ASIGNADO;
    *(registro->descripcionTarea) = NO_ASIGNADO;
    *(registro->lugarTarea) = NO_ASIGNADO;
}

//Función privada que imprime los datos de un jugador
void _imprimirJugador(tipoelem E)
{
    if (E.descripcionTarea[0] != NO_ASIGNADO && E.lugarTarea[0] != NO_ASIGNADO && E.rol != NO_ASIGNADO)
    {
        printf("%s\tRol: %c  Tarea: |%s\t| en |%s\t|\n", E.nombreJugador, E.rol, E.descripcionTarea, E.lugarTarea);
    }
    else if (E.nombreJugador[0] != NO_ASIGNADO)
    {
        printf("%s\n", E.nombreJugador);
    }
    else
    {
        printf("Datos del jugador erróneos\n");
    }
}

// Función auxiliar para _numeroNodos
void _auxContador(abb nodo, int *c)
{
    if (!es_vacio(nodo))
    {
        (*c)++;
        _auxContador(izq(nodo), c);
        _auxContador(der(nodo), c);
    }
}

// Devuelve el número de nodos de un árbol
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
            _imprimirJugador(jugador);
        }
        _imprimirPorHabitacion(der(A), habitacion);
    }
}

// Función auxiliar para _buscarPorIndice
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
// Devuelve el jugador correspondiente al jugador número indiceBuscado, del árbol A, en *resultado
void _buscarPorIndice(abb A, int indiceBuscado, tipoelem *resultado)
{
    int *indiceActual = (int *)malloc(sizeof(int));
    *indiceActual = 0;
    _auxBuscarPorIndice(A, indiceBuscado, indiceActual, resultado);
    free(indiceActual);
}

void _asignarTareaAleatoriamente(tipoelem *jugador)
{
    int numTarea, numHabitacion; // Aquí guardaremos el número de la tarea (coincide con el PDF) y la habitación (su índice en el array de abajo, no tiene que coincidir con el PDF) en que la va a realizar el jugador

    char **tarea = (char **)malloc(sizeof(char **) * 8); // Este es el listado de las posibles tareas
    tarea[0] = "Alinear la salida del motor";
    tarea[1] = "Calibrar distribuidor";
    tarea[2] = "Descargar datos/subir datos";
    tarea[3] = "Desviar energía";
    tarea[4] = "Encender escudos";
    tarea[5] = "Estabilizar dirección";
    tarea[6] = "Limpiar el filtro O2";
    tarea[7] = "Mapa de navegación";

    char **habitaciones = (char **)malloc(sizeof(char **) * 9); // Este es el listado de las habitaciones
    habitaciones[0] = "Armería";
    habitaciones[1] = "Cafetería";
    habitaciones[2] = "Comunicaciones";
    habitaciones[3] = "Electricidad";
    habitaciones[4] = "Escudos";
    habitaciones[5] = "Motores";
    habitaciones[6] = "Navegación";
    habitaciones[7] = "O2";
    habitaciones[8] = "Seguridad";

    numTarea = _aleatorio(1, 8);                                                                         // Elegimos una tarea al azar
    strncpy(jugador->descripcionTarea, tarea[numTarea - 1], sizeof(char) * (strlen(tarea[numTarea - 1]) + 1)); // Copiamos la tarea en el campo descripcionTarea

    switch (numTarea)
    {
    case 1:
        numHabitacion = 5;
        break;
    case 2:
        numHabitacion = 3;
        break;
    case 3:
        switch (_aleatorio(1, 5))
        {
        case 1:
            numHabitacion = 1;
            break;
        case 2:
            numHabitacion = 2;
            break;
        case 3:
            numHabitacion = 0;
            break;
        case 4:
            numHabitacion = 3;
            break;
        case 5:
            numHabitacion = 6;
            break;
        }
        break;
    case 4:
        switch (_aleatorio(1, 7))
        {
        case 1:
            numHabitacion = 6;
            break;
        case 2:
            numHabitacion = 7;
            break;
        case 3:
            numHabitacion = 8;
            break;
        case 4:
            numHabitacion = 0;
            break;
        case 5:
            numHabitacion = 2;
            break;
        case 6:
            numHabitacion = 4;
            break;
        case 7:
            numHabitacion = 5;
            break;
        }
        break;
    case 5:
        numHabitacion = 4;
        break;
    case 6:
        numHabitacion = 6;
        break;
    case 7:
        numHabitacion = 7;
        break;
    case 8:
        numHabitacion = 6;
        break;
    }

    strncpy(jugador->lugarTarea, habitaciones[numHabitacion], sizeof(char) * (strlen(habitaciones[numHabitacion]) + 1)); // Copiamos la habitación de la tarea en el campo lugarTarea
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
    registro.nombreJugador[0] = NO_ASIGNADO;
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
    registro.nombreJugador[0] = NO_ASIGNADO;
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
        _imprimirJugador(jugador);
        listadoJugadores(der(A));
    }
}

//Función que genera los datos de una partida: jugadores, roles y tareas
void generarPartida(abb *Arbol)
{
    int numJugadores, numImpostores, contador; // Variables auxiliares que usaremos para hacer la asignación
    char opcion;                               // Para el menú
    tipoelem jugador;                          // Variable auxiliar para almacenar un jugador (se reasignará según sea conveniente)
    abb arbolJuego;                            // Aquí guardaremos los jugadores de la partida
    crear(&arbolJuego);                        // Creamos el árbol de los jugadores de la partida
    char* nombreJugador = NULL;

    printf("En esta versión del programa, los participantes se seleccionarán aleatoriamente\n");

    do
    {
        printf("Número de participantes (4-10): ");
        scanf("%d", &numJugadores);
    } while (!(numJugadores >= 4 && numJugadores <= 10));

    opcion = '\0'; // Así nos saltamos este menú, ya que los jugadores se eligen automáticamente
    while (opcion != 's' && opcion != 'S' && opcion != 'n' && opcion != 'N')
    {
        printf("Quieres hacer automáticamente el reparto de jugadores? (s/n): ");
        scanf(" %c", &opcion);
    }

    if (opcion == 's' || opcion == 'S')
    {                                              // Repartir los jugadores automáticamente
        numImpostores = round(numJugadores / 5.0); // Calculamos el número de impostores. Los x primeros jugadores que entren en el árbol serán impostores (esto podemos hacerlo porque entran en orden aleatorio, así que es lo mismo que que repartamos los roles después de que estén en el árbol)

        printf("Jugadores seleccionados\n"); // Cuando elijamos a un jugador, iremos imprimiendo su nombre en la lista

        contador = 0;
        while (contador < numJugadores)
        {
            _buscarPorIndice(*Arbol, _aleatorio(1, _numeroNodos(*Arbol)), &jugador); // Buscamos el jugador que en un recorrido inorden está en la posición que devuelve _aleatorio(1, _numeroNodos(*Arbol))
            if (!es_miembro(arbolJuego, jugador))                                    // Solo lo añadimos al árbol si no está ya
            {
                if (contador < numImpostores)
                { // Si aún no tenemos cubierto el cupo de impostores, le asignamos a este jugador el rol de impostor (los primeros en entrar al árbol tendrán ese rol)
                    jugador.rol = ROL_IMPOSTOR;
                }
                else
                {
                    jugador.rol = ROL_TRIPULANTE;
                }
                _asignarTareaAleatoriamente(&jugador);
                insertar(&arbolJuego, jugador);
                contador++; // Insertamos el siguiente jugador
                printf("%s\n", jugador.nombreJugador);
            }
        }

        printf("\nNúmero de impostores: %d\n", numImpostores);

        printf("\nEstado final de los jugadores:\n");
        listadoJugadores(arbolJuego);
    }
    else
    { // Repartir los jugadores manualmente
        contador = 0;
        while (contador < numJugadores)
        {
            printf("Nombre de jugador: ");
            scanf("%s", nombreJugador);
            if (nombreJugador[0] == '@') {
                buscar_nodo(*Arbol, nombreJugador, &jugador);
                if (!es_miembro(arbolJuego, jugador)) {
                    _asignarTareaAleatoriamente(&jugador);
                    insertar(&arbolJuego, jugador);
                    contador++;
                }
            }
        }


    }

        printf("\nEstado final de los jugadores:\n");
        listadoJugadores(arbolJuego);
    destruir(&arbolJuego);
}

//Función que imprime los datos de un usuario cuyo nombre se introduce por teclado
void consultarJugador(abb Arbol)
{
    tipoelem registro;
    registro.nombreJugador[0] = NO_ASIGNADO;
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
    habitaciones[5] = "Motores";
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
