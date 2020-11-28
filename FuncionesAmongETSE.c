#include <stdio.h>
#include <stdlib.h> // necesaria para rand()
#include <string.h> // para comparar cadenas
#include <math.h>   // para calcular el número de impostores con round()
#include <ctype.h>  // para toupper()

#include "abb.h"
#include "FuncionesAmongETSE.h"
#include "cola.h"
#include "FuncionesAEMaps.h"

//Función privada que genera un número aleatorio entre inf y sup
unsigned int _aleatorio(int inf, int sup)
{
    return (rand() % (sup - inf + 1)) + inf;
}

// Devuelve una cola con las tareas copiadas de otra
cola _copiarTareas(cola colaTareas)
{
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

void _copiarJugadoresArbol(abb orig, abb dest)
{
    tipoelem jug;
    if (!es_vacio(orig))
    {
        _copiarJugadoresArbol(izq(orig), dest);
        leer(orig, &jug);
        jug.tareas = _copiarTareas(jug.tareas); // Las tareas en el árbol orginal van a ser una copia de las de este arbol, no las mismas
        modificar(dest, jug);
        _copiarJugadoresArbol(der(orig), dest);
    }
}

//Función privada para inicializar los datos de un jugador, necesaria en varias funciones públicas
void _inicializarJugador(tipoelem *registro)
{ //inicializa los campos rol,descripcionTarea y lugarTarea
    registro->rol = NO_ASIGNADO;
    crear_cola(&(registro->tareas));
    registro->distancia = 0;
    registro->siguienteHabitacion[0] = NO_ASIGNADO;
    registro->ultimaHabitacion[0] = NO_ASIGNADO;
    registro->tiempoRestanteTarea = -1;
}

int _jugadorEstaEnUnaHabitacion(tipoelem jugador)
{
    return jugador.siguienteHabitacion[0] != NO_ASIGNADO && jugador.distancia == 0;
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

// Hacemos un recorrido en preorden del arbol guardando los jugadores en el archivo
void _guardarJugadoresArbol(FILE *fp, abb A)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        leer(A, &jugador);
        fprintf(fp, "%s\n", jugador.nombreJugador);
        _guardarJugadoresArbol(fp, der(A));
        _guardarJugadoresArbol(fp, izq(A));
    }
}

void _matarJugador(tipoelem jugador, abb A)
{
    jugador.rol = ROL_KILLED;
    modificar(A, jugador);
}

// Imprime los elementos de una cola de tareas
void _imprimirTareas(cola *tareas)
{
    cola colaAux;
    crear_cola(&colaAux);
    printf("Tareas:\n");
    while (!es_vacia_cola(*tareas)) // Vamos leyendo las tareas, para eso las metemos en una cola auxiliar
    {
        printf("\t\t\t\t|%-30s| en |%-15s|\n", primero(*tareas).descripcionTarea, primero(*tareas).lugarTarea);
        insertar_cola(&colaAux, primero(*tareas));
        suprimir_cola(tareas);
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

// Retira del mapa a los jugadores con el rol indicado que están en la habitación indicada
void _retirarJugsPorHabitacion(abb A, char rol, char *habitacion)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _retirarJugsPorHabitacion(izq(A), rol, habitacion);
        leer(A, &jugador);
        if (jugador.rol == rol && strcmp(jugador.siguienteHabitacion, habitacion) == 0)
        { // Borramos la información de este jugador, porque concide con los criterios que pasamos a la función
            while (!es_vacia_cola(jugador.tareas))
            {
                suprimir_cola(&(jugador.tareas));
            }
            destruir_cola(&(jugador.tareas));
            _inicializarJugador(&jugador);
            modificar(A, jugador);
        }
        _retirarJugsPorHabitacion(der(A), rol, habitacion);
    }
}

// Tras mostrar el menú, ya sabemos qué jugadores han muerto, así que para que otros jugadores no se encuentren sus cadáveres y los reporten, retiramos del mapa todos los cadáveres por descubrir
void _retirarJugsMuertos(abb A)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _retirarJugsMuertos(izq(A));
        leer(A, &jugador);
        if (jugador.rol == ROL_KILLED && _jugadorEstaEnUnaHabitacion(jugador))
        { // Borramos la información de este jugador, porque concide con los criterios que pasamos a la función
            while (!es_vacia_cola(jugador.tareas))
            {
                suprimir_cola(&(jugador.tareas));
            }
            destruir_cola(&(jugador.tareas));
            _inicializarJugador(&jugador);
            modificar(A, jugador);
        }
        _retirarJugsMuertos(der(A));
    }
}

// Imprime los nombres de los jugadores con el rol seleccionado que se encuentran en la habitación indicada
void _imprimirJugsPorHabitacion(abb A, char rol, char *habitacion)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _imprimirJugsPorHabitacion(izq(A), rol, habitacion);
        leer(A, &jugador);
        if (jugador.rol == rol && _jugadorEstaEnUnaHabitacion(jugador) && strcmp(jugador.siguienteHabitacion, habitacion) == 0) // Si el jugador es del rol que seleccionamos, y está en la habitación que seleccionamos (jugador.siguienteHabitacion=habitacion y jugador.distancia=0)
        {
            printf(" %s", jugador.nombreJugador);
        }
        _imprimirJugsPorHabitacion(der(A), rol, habitacion);
    }
}

int _numJugsPorHabitacion(abb A, char rol, char *habitacion)
{
    int numero = 0;
    tipoelem jugador;
    if (!es_vacio(A))
    {
        numero += _numJugsPorHabitacion(izq(A), rol, habitacion);
        leer(A, &jugador);
        if (jugador.rol == rol && _jugadorEstaEnUnaHabitacion(jugador) && strcmp(jugador.siguienteHabitacion, habitacion) == 0) // Si el jugador es del rol que seleccionamos, y está en la habitación que seleccionamos (jugador.siguienteHabitacion=habitacion y jugador.distancia=0)
        {
            numero++;
        }
        numero += _numJugsPorHabitacion(der(A), rol, habitacion);
    }
    return numero;
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

    char **habitaciones = (char **)malloc(sizeof(char **) * 10); // Este es el listado de las habitaciones
    habitaciones[0] = "Armeria";
    habitaciones[1] = "Cafeteria";
    habitaciones[2] = "Comunicacion";
    habitaciones[3] = "Electricidad";
    habitaciones[4] = "Escudos";
    habitaciones[5] = "MotorSuperior";
    habitaciones[6] = "Navegacion";
    habitaciones[7] = "O2";
    habitaciones[8] = "Seguridad";
    habitaciones[9] = "MotorInferior";

    numTarea = _aleatorio(1, 8);                                                                                                   // Elegimos una descripcionTarea al azar
    strncpy(tarea->descripcionTarea, descripcionTarea[numTarea - 1], sizeof(char) * (strlen(descripcionTarea[numTarea - 1]) + 1)); // Copiamos la descripcionTarea en el campo descripcionTarea

    switch (numTarea)
    {
    case 1:
        switch (_aleatorio(1, 2))
        {
        case 1:
            numHabitacion = 5;
            break;
        case 2:
            numHabitacion = 9;
            break;
        }
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
        switch (_aleatorio(1, 8))
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
        case 8:
            numHabitacion = 9;
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

void _auxMatarJugPorIndiceHabitacion(abb A, int jugMatar, int *i, char rol, char *habitacion)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _auxMatarJugPorIndiceHabitacion(izq(A), jugMatar, i, rol, habitacion);
        leer(A, &jugador);
        if (jugador.rol == rol && !es_vacia_cola(jugador.tareas) && strcmp(primero(jugador.tareas).lugarTarea, habitacion) == 0)
        {
            (*i)++;
            if (*i == jugMatar)
            {
                _matarJugador(jugador, A);
                // No nos interesa mostrar esto por consola para mantener el misterio
                //printf("Ha muerto %s.\n", jugador.nombreJugador);
            }
        }
        _auxMatarJugPorIndiceHabitacion(der(A), jugMatar, i, rol, habitacion);
    }
}

void _matarJugPorIndiceHabitacion(abb A, int jugMatar, char rol, char *habitacion)
{
    int i = 0;
    _auxMatarJugPorIndiceHabitacion(A, jugMatar, &i, rol, habitacion);
}

// Recorremos todo el árbol de jugadores buscando a los impostores, y por cada uno matamos a algún jugador (o no), depende de un factor aleatorio
void _auxEjecutarTick(abb arbolJugadores, abb arbolRecorrido, int *mostrarMenuMuerteJugador)
{
    int numJugs, jugMatar;
    tipoelem jugador;
    if (!es_vacio(arbolRecorrido))
    {
        _auxEjecutarTick(arbolJugadores, izq(arbolRecorrido), mostrarMenuMuerteJugador);
        _auxEjecutarTick(arbolJugadores, der(arbolRecorrido), mostrarMenuMuerteJugador);
        leer(arbolRecorrido, &jugador);
        if (jugador.rol != ROL_KILLED && _jugadorEstaEnUnaHabitacion(jugador))
        { // Si el jugador está en una habitación, puede que haya un jugador muerto en esa habitación. Lo comprobamos y en caso afirmativo, ponemos el flag de mostrarMenuMuerteJugador a 1 e imprimimos por pantalla quién se ha encontrado el cadáver
            numJugs = _numJugsPorHabitacion(arbolJugadores, ROL_KILLED, jugador.siguienteHabitacion);
            if (numJugs > 0)
            {
                *mostrarMenuMuerteJugador = 1;
                if (numJugs == 1)
                {
                    printf(COLOR_BLUE "%s" COLOR_RESET " ha reportado la muerte de" COLOR_MAGENTA, jugador.nombreJugador);
                    _imprimirJugsPorHabitacion(arbolJugadores, ROL_KILLED, jugador.siguienteHabitacion);
                    printf(COLOR_RESET " en " COLOR_CYAN "%s" COLOR_RESET ".\n", jugador.siguienteHabitacion);
                }
                else if (numJugs > 1)
                {
                    printf(COLOR_BLUE "%s" COLOR_RESET " ha reportado las muertes de" COLOR_MAGENTA, jugador.nombreJugador);
                    _imprimirJugsPorHabitacion(arbolJugadores, ROL_KILLED, jugador.siguienteHabitacion);
                    printf(COLOR_RESET " en " COLOR_CYAN "%s" COLOR_RESET ".\n", jugador.siguienteHabitacion);
                }
                _retirarJugsPorHabitacion(arbolJugadores, ROL_KILLED, jugador.siguienteHabitacion);
            }
        }
        if (jugador.rol == ROL_IMPOSTOR && _jugadorEstaEnUnaHabitacion(jugador))
        { // Por cada impostor...
            numJugs = _numJugsPorHabitacion(arbolJugadores, ROL_TRIPULANTE, jugador.siguienteHabitacion);
            if (numJugs > 0)
            { // Miramos si hay algún tripulante en la habitación del impostor. Si lo hay, lo matamos, pero sólo si hay uno, porque habiendo dos se delata
                if (_aleatorio(1, 4) != 1)
                { // Sucederá 2/3 de las veces, es decir, el impostor no siempre matará a alguien en su habitación
                    jugMatar = _aleatorio(1, numJugs);
                    _matarJugPorIndiceHabitacion(arbolJugadores, jugMatar, ROL_TRIPULANTE, jugador.siguienteHabitacion);
                }
            }
        }
    }
}

// Cuenta el número de jugadores de un árbol que tienen un rol determinado
int _contarRol(abb A, char rol)
{
    int numero = 0;
    tipoelem jugador;
    if (!es_vacio(A))
    {
        numero += _contarRol(izq(A), rol);
        leer(A, &jugador);
        if (jugador.rol == rol)
        {
            numero++;
        }
        numero += _contarRol(der(A), rol);
    }
    return numero;
}

// Coge un jugador, que ha llegado a una habitación, y le asigna la siguiente a la que ir
void _auxActualizarDistanciaHabitacion(tipoelem *jugador, grafo G)
{
    tipovertice habActual;
    tipovertice habSiguiente;
    tipovertice habDestino;
    // Aquí pueden darse 2 casos: que el jugador haya llegado a una habitación, pero solo esté pasando por allí (lo pueden matar pese a todo); o que esta habitación a la que llega sea la de la tarea, en cuyo caso avanzamos la cola de tareas
    if (strncmp(primero(jugador->tareas).lugarTarea, jugador->siguienteHabitacion, L_HABITACION) == 0)
    { // Si esta es la habitación de la tarea...
        if (jugador->tiempoRestanteTarea == -1)
        { // Si tiempoRestanteTarea es -1, es que acaba de llegar a la habitación. Le asignamos una cantidad de ticks de tiempo entre TIEMPO_MIN_TAREA y TIEMPO_MAX_TAREA para realizar la tarea.
            jugador->tiempoRestanteTarea = _aleatorio(TIEMPO_MIN_TAREA, TIEMPO_MAX_TAREA);
        }
        else if (jugador->tiempoRestanteTarea == 0)
        {                                                                                          // Si ha agotado el tiempo de su tarea, la borramos de la cola de tareas
            strncpy(jugador->ultimaHabitacion, primero(jugador->tareas).lugarTarea, L_HABITACION); // ultimaHabitacion = esta habitación
            suprimir_cola(&(jugador->tareas));
            jugador->tiempoRestanteTarea = -1;
        }
        else
        { // Si aún le faltan algunos ticks para acabar la tarea, solo hacemos avanzar el tiempo
            jugador->tiempoRestanteTarea--;
        }
    }

    if (!es_vacia_cola(jugador->tareas))
    { // Si aún quedan tareas por realizar...
        if (strncmp(jugador->siguienteHabitacion, primero(jugador->tareas).lugarTarea, L_HABITACION) == 0)
        { // La habitación en la que está la siguiente tarea del jugador es la actual
            jugador->distancia = 0;
        }
        else
        {
            strncpy(jugador->ultimaHabitacion, jugador->siguienteHabitacion, L_HABITACION); // ultimaHabitacion = siguienteHabitacion
            strncpy(habActual.habitacion, jugador->ultimaHabitacion, L_HABITACION);
            strncpy(habDestino.habitacion, primero(jugador->tareas).lugarTarea, L_HABITACION);

            habSiguiente = verticeSiguiente(G, habActual, habDestino, jugador->rol); // Obtenemos a qué habitación tendría que ir ahora nuestro jugador para acercarse a la de su tarea

            switch (jugador->rol) // Escribimos la distancia a la siguiente habitación en el jugador
            {
            case 'T':
                jugador->distancia = distancia_T(G, posicion(G, habActual), posicion(G, habSiguiente));
                break;
            case 'I':
                jugador->distancia = distancia_I(G, posicion(G, habActual), posicion(G, habSiguiente));
                break;
            }

            strncpy(jugador->siguienteHabitacion, habSiguiente.habitacion, L_HABITACION); // Escribimos la siguiente habitación en el jugador
        }
    }
}

// Ejecuta un tick del tiempo del juego:
// Mueve los jugadores una unidad de tiempo, y los impostores matan a los que estén en sus mismas habitaciones
int _ejecutarTick(abb A)
{
    int mostrarMenuMuerteJugador = 0;
    _auxEjecutarTick(A, A, &mostrarMenuMuerteJugador);
    return mostrarMenuMuerteJugador;
}

// Avanza un tick de tiempo el juego
void _siguienteTick(abb A, grafo G)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _siguienteTick(izq(A), G);
        leer(A, &jugador);
        if (!es_vacia_cola(jugador.tareas) && jugador.rol != ROL_KILLED)
        {
            if (_jugadorEstaEnUnaHabitacion(jugador)) // El jugador ha llegado a una habitación, actualizamos la siguiente a la que le toca ir
            {
                _auxActualizarDistanciaHabitacion(&jugador, G);
            }
            else
            { // El jugador está en tránsito, así que decrementamos su distancia
                jugador.distancia--;
            }
        }
        modificar(A, jugador);
        _siguienteTick(der(A), G);
    }
}

// Cuenta el número de jugadores de un árbol que tienen un rol determinado
int _contarJugadoresConTareas(abb A, char rol)
{
    int numero = 0;
    tipoelem jugador;
    if (!es_vacio(A))
    {
        numero += _contarJugadoresConTareas(izq(A), rol);
        leer(A, &jugador);
        if (jugador.rol == rol && !es_vacia_cola(jugador.tareas))
        {
            numero++;
        }
        numero += _contarJugadoresConTareas(der(A), rol);
    }
    return numero;
}

// Comprueba si se cumple alguna de las condiciones de victoria y, en ese caso, devuelve VICTORIA_IMPOSTORES, VICTORIA_TRIPULANTES o EMPATE. Si no, devuleve falso
int _comprobarVictoria(abb A)
{
    if (_contarRol(A, ROL_IMPOSTOR) == 0)
    {
        return (VICTORIA_TRIPULANTES);
    }
    if (_contarRol(A, ROL_TRIPULANTE) <= _contarRol(A, ROL_IMPOSTOR))
    {
        return (VICTORIA_IMPOSTORES);
    }
    if (_contarJugadoresConTareas(A, ROL_TRIPULANTE) == 0)
    {
        return (VICTORIA_TRIPULANTES);
    }
    return 0;
}

// Recorre un árbol llamando a _inicializarJugador() en cada nodo, y guardando los cambios en el árbol
void _limpiarDatos(abb A)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _limpiarDatos(izq(A));
        leer(A, &jugador);
        if (jugador.tareas != NULL)
        {
            destruir_cola(&(jugador.tareas));
        }
        _inicializarJugador(&jugador);
        modificar(A, jugador);
        _limpiarDatos(der(A));
    }
}

//Función para leer el archivo de disco
void leerArchivoJugadores(abb *A)
{
    tipoelem registro;
    FILE *fp;
    fp = fopen("jugadores.txt", "rt");
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

// Igual que listadoJugadores, pero imprimiendo sólo el nombre de los jugadores y la última habitación en la que han estado
void _listadoJugadoresVivosYUltimasHabitaciones(abb A)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _listadoJugadoresVivosYUltimasHabitaciones(izq(A));
        leer(A, &jugador);
        if (jugador.rol == ROL_IMPOSTOR || jugador.rol == ROL_TRIPULANTE)
        {
            if (es_vacia_cola(jugador.tareas))
            {
                printf("%-15s  Sin tarea\n", jugador.nombreJugador);
            }
            else
            {
                printf("%-15s  | %-15s | %-15s |\n", jugador.nombreJugador, jugador.ultimaHabitacion, jugador.distancia == 0 ? jugador.siguienteHabitacion : "En transito");
            }
        }
        _listadoJugadoresVivosYUltimasHabitaciones(der(A));
    }
}

// Igual que listadoJugadores, pero imprimiendo sólo el nombre de los jugadores
void _listadoNombresJugadores(abb A)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _listadoNombresJugadores(izq(A));
        leer(A, &jugador);
        printf("%s\n", jugador.nombreJugador);
        _listadoNombresJugadores(der(A));
    }
}

void _prepararJugadorInicio(tipoelem *jugador, grafo G)
{
    strncpy(jugador->ultimaHabitacion, "Cafeteria", L_HABITACION);
    strncpy(jugador->siguienteHabitacion, "Cafeteria", L_HABITACION);
    jugador->distancia = 0;
    _auxActualizarDistanciaHabitacion(jugador, G);
}

//Función que genera los datos de una partida: jugadores, roles y tareas
void jugar(abb *Arbol, grafo G)
{
    int numJugadores, numImpostores, contador, victoria; // Variables auxiliares que usaremos para hacer la asignación
    char opcion, opcionAnunciarImpostores;               // Para el menú
    tipoelem jugador;                                    // Variable auxiliar para almacenar un jugador (se reasignará según sea conveniente)
    abb arbolJuego;                                      // Aquí guardaremos los jugadores de la partida
    crear(&arbolJuego);                                  // Creamos el árbol de los jugadores de la partida
    char *nombreJugador = NULL;

    _limpiarDatos(*Arbol); // Limpiamos el árbol antes de generar una nueva partida

    do
    {
        printf("Número de participantes (4-15): ");
        scanf(" %d", &numJugadores);
    } while (!(numJugadores >= 4 && numJugadores <= 15));

    printf("Deseas anunciar si los jugadores expulsados son impostores o no? (s/N): "); // Normalmente, la partida es "a ciegas": no anunciamos si el jugador que elegimos echar de la nave es un impostor o no, para añadir incertidumbre hasta el final. Pese a todo, se puede desactivar esta opción
    scanf(" %c", &opcionAnunciarImpostores);
    opcionAnunciarImpostores = toupper(opcionAnunciarImpostores); // Cualquier opción que no sea 'S' se interpretará como que no queremos que se anuncien

    do
    {
        printf("Quieres hacer automáticamente el reparto de jugadores? (s/n): ");
        scanf(" %c", &opcion);
        opcion = toupper(opcion);
    } while (opcion != 'S' && opcion != 'N');

    numImpostores = round(numJugadores / 5.0); // Calculamos el número de impostores. Los x primeros jugadores que entren en el árbol serán impostores (esto podemos hacerlo porque entran en orden aleatorio, así que es lo mismo que que repartamos los roles después de que estén en el árbol)

    switch (opcion)
    {
    case 'S': // Repartir los jugadores automáticamente
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
                _prepararJugadorInicio(&jugador, G);
                insertar(&arbolJuego, jugador);
                contador++; // Insertamos el siguiente jugador
            }
        }
        break;

    case 'N': // Repartir los jugadores manualmente.
        contador = 0;
        while (contador < numJugadores)
        {
            nombreJugador = (char *)malloc(sizeof(char) * (L_NOMBRE + 1)); // Reservamos memoria para guardar el nombre del jugador
            printf("Nombre de jugador: ");
            scanf(" %s", nombreJugador);
            if (nombreJugador[0] == '@')
            {
                buscar_nodo(*Arbol, nombreJugador, &jugador);
                if (strcmp(jugador.nombreJugador, nombreJugador) == 0 && es_miembro(*Arbol, jugador))
                {
                    if (!es_miembro(arbolJuego, jugador))
                    {
                        jugador.rol = ROL_TRIPULANTE; // Todos van a ser tripulantes por defecto, y luego repartiremos aleatoriamente los roles de impostor
                        _asignarTareas(jugador);
                        insertar(&arbolJuego, jugador);
                        contador++;
                        printf("Jugador registrado correctamente.\n");
                    }
                    else
                    {
                        printf("El jugador ya está registrado.\n");
                    }
                }
                else
                {
                    printf("El jugador no está registrado.\n");
                }
            }
        }

        contador = 0;
        while (contador < numImpostores)
        {                                                                        // Asignamos aleatoriamente los roles de impostor
            _buscarPorIndice(arbolJuego, _aleatorio(1, numJugadores), &jugador); // Buscamos el jugador que en un recorrido inorden está en la posición que devuelve _aleatorio(1, _numeroNodos(*Arbol))
            if (jugador.rol != ROL_IMPOSTOR)
            {
                jugador.rol = ROL_IMPOSTOR;
                modificar(arbolJuego, jugador); // Modificamos el rol en el árbol local de esta función
                contador++;
                buscar_nodo(*Arbol, jugador.nombreJugador, &jugador);
                jugador.rol = ROL_IMPOSTOR;
            }
        }
        free(nombreJugador); // Liberamos la memoria

        break;
    }

    printf("\n" COLOR_RED "=========================" COLOR_RESET "\n");
    printf(COLOR_RED "======== PARTIDA ========" COLOR_RESET "\n");
    printf(COLOR_RED "=========================" COLOR_RESET "\n");
    printf("\nNúmero de impostores: %d\n", numImpostores);

    printf("\nEstado final de los jugadores:\n\n");
    _listadoNombresJugadores(arbolJuego);
    printf("\n");

    nombreJugador = (char *)malloc(sizeof(char) * (L_NOMBRE + 1));
    do
    {
        if (_ejecutarTick(arbolJuego))
        { // Si se ha encontrado un jugador muerto en este tick...
            printf("\nJugadores        | Ultima habitac. | Hab. actual     |\n------------------------------------------------------\n");
            _listadoJugadoresVivosYUltimasHabitaciones(arbolJuego);
            printf("\n");

            printf("\nQuien es el impostor? ");
            scanf(" %s", nombreJugador); // Le pedimos un nombre de jugador para eliminar al usuario, si no es un nombre válido se supone que queremos pasar el turno
            if (nombreJugador[0] == '@')
            {
                buscar_nodo(arbolJuego, nombreJugador, &jugador);
                if (strcmp(nombreJugador, jugador.nombreJugador) == 0)
                {                                                                     // Si hemos encontrado el jugador...
                    if (jugador.rol == ROL_IMPOSTOR || jugador.rol == ROL_TRIPULANTE) // El jugador tiene que estar vivo
                    {
                        if (opcionAnunciarImpostores == 'S')
                        {                                                                                                       // Si activamos la opción de anunciar impostores...
                            printf("\n" COLOR_BLUE "%s" COLOR_RESET " %sera un impostor.\n\n", jugador.nombreJugador, jugador.rol == ROL_IMPOSTOR ? "" : "no "); // Anunciamos si el jugador era o no era un impostor
                        }
                        destruir_cola(&(jugador.tareas)); // Sacamos al jugador del curso de la partida, aunque no borramos su entrada en el árbol de juego
                        _inicializarJugador(&jugador);
                        modificar(arbolJuego, jugador);
                    }
                    else
                    {
                        printf("Ese jugador ya está muerto. No se ha matado a nadie.\n");
                    }
                }
            }
            else
            {
                printf("No se ha matado a nadie.\n");
            }
            _retirarJugsMuertos(arbolJuego);
        }

        _siguienteTick(arbolJuego, G);
        victoria = _comprobarVictoria(arbolJuego);
    } while (victoria == 0);
    free(nombreJugador);

    if (victoria == VICTORIA_IMPOSTORES)
    {
        printf("\nHan ganado los impostores.\n");
    }
    else if (victoria == VICTORIA_TRIPULANTES)
    {
        printf("\nHan ganado los tripulantes.\n");
    }

    _copiarJugadoresArbol(arbolJuego, *Arbol);

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
        if (!es_vacia_cola(registro.tareas))
        {
            printf("Próxima tarea a realizar: |%-30s| en |%-15s|\n", primero(registro.tareas).descripcionTarea, primero(registro.tareas).lugarTarea);
        }
        else
        {
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

void guardarArchivoJugadores(abb A)
{
    FILE *fp;
    fp = fopen("jugadores.txt", "wt");
    if (fp)
    {
        _guardarJugadoresArbol(fp, A);
        fclose(fp);
        printf("Jugadores guardados correctamente.\n");
    }
}
