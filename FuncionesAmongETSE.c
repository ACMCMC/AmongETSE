#include <stdio.h>
#include <stdlib.h> //necesaria para rand()
#include <string.h> //para comparar cadenas
#include <math.h>   //para calcular el número de impostores con round()

#include "abb.h"
#include "FuncionesAmongETSE.h"
#include "cola.h"

#define ROL_IMPOSTOR 'I'
#define ROL_TRIPULANTE 'T'
#define NO_ASIGNADO '\0'

//Función privada que genera un número aleatorio entre inf y sup
unsigned int _aleatorio(int inf, int sup)
{
    return (rand() % (sup - inf + 1)) + inf;
}

// Devuelve una cola con las tareas copiadas de otra
cola _copiarTareas(cola colaTareas) {
    cola newCola, colaAux; // Usamos dos colas, una auxiliar para vaciar allí las tareas de la cola original y otra que va a ser la cola nueva
    crear_cola(&newCola);
    crear_cola(&colaAux);
    tipoelemCola elem;
    while (!es_vacia_cola(colaTareas)) // Vaciamos las tareas en la cola auxiliar
    {
        elem = primero(colaTareas);
        suprimir_cola(&colaTareas);
        insertar_cola(&colaAux, elem);
    }
    while (!es_vacia_cola(colaAux)) // Volvemos a meter las tareas de la cola auxiliar en la cola original, y en la nueva
    {
        insertar_cola(&colaTareas, primero(colaAux));
        insertar_cola(&newCola, primero(colaAux));
        suprimir_cola(&colaAux);
    }
    destruir_cola(&colaAux);
    return newCola;
}

//Función privada para inicializar los datos de un jugador, necesaria en varias funciones públicas
void _inicializarJugador(tipoelem *registro)
{ //inicializa los campos rol,descripcionTarea y lugarTarea
    registro->rol = NO_ASIGNADO;
    crear_cola(&(registro->tareas));
}

void _guardarJugadoresArbol(FILE * fp, abb A) {
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _guardarJugadoresArbol(fp, izq(A));
        leer(A, &jugador);
        fprintf(fp, "%s\n", jugador.nombreJugador);
        _guardarJugadoresArbol(fp, der(A));
    }
}

// Imprime los elementos de una cola de tareas
void _imprimirTareas(cola *tareas)
{
    cola colaAux;
    crear_cola(&colaAux);
    tipoelemCola elem;
    printf("Tareas:\n");
    while (!es_vacia_cola(*tareas)) // Vamos leyendo las tareas, para eso las metemos en una cola auxiliar
    {
        elem = primero(*tareas);
        printf("\t\t\t\t|%-30s| en |%-15s|\n", elem.descripcionTarea, elem.lugarTarea);
        suprimir_cola(tareas);
        insertar_cola(&colaAux, elem);
    }
    while (!es_vacia_cola(colaAux)) // Volvemos a meter las tareas de la cola auxiliar en la cola original
    {
        insertar_cola(tareas, primero(colaAux));
        suprimir_cola(&colaAux);
    }
    destruir_cola(&colaAux);
}

//Función privada que imprime los datos de un jugador
void _imprimirJugador(tipoelem E)
{
    if (E.nombreJugador[0] != NO_ASIGNADO)
    {
        printf("%-15s", E.nombreJugador);
        if (E.rol != NO_ASIGNADO)
        {
            printf("  Rol: %c  ", E.rol);
            _imprimirTareas(&(E.tareas));
        }
        printf("\n");
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
    cola colaAux;
    crear_cola(&colaAux);
    tipoelemCola tarea;
    int flag_imprimir = 0; // Lo usaremos para saber si hay que imprimir este jugador o no. Usamos un flag porque no imprimimos todo el jugador cada vez que encontramos una tarea que se desarrolle en la habitación buscada, sino que imprimimos el jugador una sola vez si alguna de las tareas se desarrolla en esa habitación.
    if (!es_vacio(A))
    {
        _imprimirPorHabitacion(izq(A), habitacion);
        leer(A, &jugador);
        while (!es_vacia_cola(jugador.tareas)) // Vamos leyendo las tareas, para eso las metemos en una cola auxiliar
        {
            tarea = primero(jugador.tareas);
            suprimir_cola(&(jugador.tareas));
            insertar_cola(&colaAux, tarea);
            if (strcmp(tarea.lugarTarea, habitacion) == 0)
            {
                flag_imprimir = 1;
            }
        }
        while (!es_vacia_cola(colaAux)) // Volvemos a meter las tareas de la cola auxiliar en la cola original
        {
            insertar_cola(&(jugador.tareas), primero(colaAux));
            suprimir_cola(&colaAux);
        }
        destruir_cola(&colaAux); // Liberamos la cola auxiliar
        if (flag_imprimir == 1)
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

// Asigna únicamente una tarea
void _asignarTarea(tipoelemCola *tarea)
{
    int numTarea, numHabitacion; // Aquí guardaremos el número de la descripcionTarea (coincide con el PDF) y la habitación (su índice en el array de abajo, no tiene que coincidir con el PDF) en que la va a realizar el jugador

    char **descripcionTarea = (char **)malloc(sizeof(char **) * 8); // Este es el listado de las posibles tareas
    descripcionTarea[0] = "Alinear la salida del motor";
    descripcionTarea[1] = "Calibrar distribuidor";
    descripcionTarea[2] = "Descargar datos/subir datos";
    descripcionTarea[3] = "Desviar energia";
    descripcionTarea[4] = "Encender escudos";
    descripcionTarea[5] = "Estabilizar direccion";
    descripcionTarea[6] = "Limpiar el filtro O2";
    descripcionTarea[7] = "Mapa de navegacion";

    char **habitaciones = (char **)malloc(sizeof(char **) * 9); // Este es el listado de las habitaciones
    habitaciones[0] = "Armeria";
    habitaciones[1] = "Cafeteria";
    habitaciones[2] = "Comunicaciones";
    habitaciones[3] = "Electricidad";
    habitaciones[4] = "Escudos";
    habitaciones[5] = "Motores";
    habitaciones[6] = "Navegacion";
    habitaciones[7] = "O2";
    habitaciones[8] = "Seguridad";

    numTarea = _aleatorio(1, 8);                                                                                                   // Elegimos una descripcionTarea al azar
    strncpy(tarea->descripcionTarea, descripcionTarea[numTarea - 1], sizeof(char) * (strlen(descripcionTarea[numTarea - 1]) + 1)); // Copiamos la descripcionTarea en el campo descripcionTarea

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

    strncpy(tarea->lugarTarea, habitaciones[numHabitacion], sizeof(char) * (strlen(habitaciones[numHabitacion]) + 1)); // Copiamos la habitación de la descripcionTarea en el campo lugarTarea
    free(descripcionTarea);
    free(habitaciones);
}

// Coge un jugador y le asigna NUM_TAREAS a su cola de tareas
void _asignarTareas(tipoelem jugador)
{
    int i = 0;
    tipoelemCola tarea;
    while (i < NUM_TAREAS)
    {
        _asignarTarea(&tarea);
        insertar_cola(&(jugador.tareas), tarea);
        i++;
    }
}

void _ejecutarTarea()
{
}

// Establece el rol de un jugador como 'K' (killed)
void _matarJugador()
{
}

// Devuelve el número de tripulantes que hay en una habitación
int _numTripulantesPorHabitacion(abb A, char *nombreHabitacion)
{
}

// Elimina la descripTarea del primero de la cola de tareas del jugador
void _siguienteTarea(tipoelem jugador)
{
    suprimir_cola(&(jugador.tareas));
}

// Recorre un árbol llamando a _inicializarJugador() en cada nodo, y guardando los cambios en el árbol
void _limpiarDatos(abb A)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _limpiarDatos(izq(A));
        leer(A, &jugador);
        if (jugador.tareas != NULL) {
            destruir_cola(&(jugador.tareas));
        }
        _inicializarJugador(&jugador);
        modificar(A, jugador);
        _limpiarDatos(der(A));
    }
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
void jugar(abb *Arbol)
{
    int numJugadores, numImpostores, contador; // Variables auxiliares que usaremos para hacer la asignación
    char opcion;                               // Para el menú
    tipoelem jugador;                          // Variable auxiliar para almacenar un jugador (se reasignará según sea conveniente)
    abb arbolJuego;                            // Aquí guardaremos los jugadores de la partida
    crear(&arbolJuego);                        // Creamos el árbol de los jugadores de la partida
    char *nombreJugador = NULL;

    _limpiarDatos(*Arbol); // Limpiamos el árbol antes de generar una nueva partida

    printf("En esta versión del programa, los participantes se seleccionarán aleatoriamente\n");

    do
    {
        printf("Número de participantes (4-10): ");
        scanf(" %d", &numJugadores);
    } while (!(numJugadores >= 4 && numJugadores <= 10));

    opcion = 's'; // Así nos saltamos este menú, ya que los jugadores se eligen automáticamente
    while (opcion != 's' && opcion != 'S' && opcion != 'n' && opcion != 'N')
    {
        printf("Quieres hacer automáticamente el reparto de jugadores? (s/n): ");
        scanf(" %c", &opcion);
    }

    numImpostores = round(numJugadores / 5.0); // Calculamos el número de impostores. Los x primeros jugadores que entren en el árbol serán impostores (esto podemos hacerlo porque entran en orden aleatorio, así que es lo mismo que que repartamos los roles después de que estén en el árbol)

    if (opcion == 's' || opcion == 'S')
    { // Repartir los jugadores automáticamente

        printf("\nJugadores seleccionados\n"); // Cuando elijamos a un jugador, iremos imprimiendo su nombre en la lista

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
                _asignarTareas(jugador);
                insertar(&arbolJuego, jugador);
                jugador.tareas = _copiarTareas(jugador.tareas); // Las tareas en el árbol orginal van a ser una copia de las de este arbol, no las mismas, porque como después eliminamos este árbol entonces tendríamos un segmentation fault si intentásemos acceder a las tareas de la cola original, porque estaría borrada
                modificar(*Arbol, jugador);
                contador++; // Insertamos el siguiente jugador
                printf("%s\n", jugador.nombreJugador);
            }
        }
    }
    else
    { // Repartir los jugadores manualmente. No se admite en esta versión de la práctica.
        contador = 0;
        while (contador < numJugadores)
        {
            printf("Nombre de jugador: ");
            scanf(" %s", nombreJugador);
            if (nombreJugador[0] == '@')
            {
                buscar_nodo(*Arbol, nombreJugador, &jugador);
                if (!es_miembro(arbolJuego, jugador))
                {
                    _asignarTareas(jugador);
                    insertar(&arbolJuego, jugador);
                    jugador.tareas = _copiarTareas(jugador.tareas);
                    modificar(*Arbol, jugador);
                    contador++;
                }
            }
        }
    }

    printf("\nNúmero de impostores: %d\n", numImpostores);

    printf("\nEstado final de los jugadores:\n");
    listadoJugadores(arbolJuego);

    destruir(&arbolJuego); // Destruímos el árbol al final de la función, porque es local
}

//Función que imprime los datos de un usuario cuyo nombre se introduce por teclado
void consultarTareaJugador(abb Arbol)
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
        buscar_nodo(Arbol, registro.nombreJugador, &registro);
        if (!es_vacia_cola(registro.tareas)) {
            printf("Próxima tarea a realizar: |%-30s| en |%-15s|\n", primero(registro.tareas).descripcionTarea, primero(registro.tareas).lugarTarea);
        } else {
            printf("El jugador no tiene tareas asignadas.\n");
        }
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
    habitaciones[0] = "Armeria";
    habitaciones[1] = "Cafeteria";
    habitaciones[2] = "Comunicaciones";
    habitaciones[3] = "Electricidad";
    habitaciones[4] = "Escudos";
    habitaciones[5] = "Motores";
    habitaciones[6] = "Navegacion";
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
        scanf(" %d", &numHabitacion);
    }
    numHabitacion--; // El índice en la lista de nombres es una unidad menos
    _imprimirPorHabitacion(Arbol, habitaciones[numHabitacion]);

    free(habitaciones);
}

void guardarArchivo(abb A) {
    FILE *fp;
    fp = fopen("jugadores.txt", "wt");
    if (fp)
    {
        _guardarJugadoresArbol(fp, A);
        fclose(fp);
        printf("Jugadores guardados correctamente.\n");
    }
}
