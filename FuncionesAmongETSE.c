#include <stdio.h>
#include <stdlib.h> //necesaria para rand()
#include <string.h> //para comparar cadenas
#include <math.h>   //para calcular el número de impostores con round()

#include "abb.h"
#include "FuncionesAmongETSE.h"
#include "cola.h"

#define ROL_IMPOSTOR 'I'
#define ROL_TRIPULANTE 'T'
#define ROL_KILLED 'K'
#define NO_ASIGNADO '\0'

#define VICTORIA_IMPOSTORES 1
#define VICTORIA_TRIPULANTES 2

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

int _numJugsPorHabitacion(abb A, char rol, char *habitacion)
{
    int numero = 0;
    tipoelem jugador;
    if (!es_vacio(A))
    {
        numero += _numJugsPorHabitacion(izq(A), rol, habitacion);
        leer(A, &jugador);
        if (jugador.rol == rol && !es_vacia_cola(jugador.tareas) && strcmp(primero(jugador.tareas).lugarTarea, habitacion) == 0)
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
                printf("Ha muerto %s.\n", jugador.nombreJugador);
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

// Recorremos todo el árbol de jugadores buscando a los impostores, y por cada uno matamos a algún jugador (o no). El recorrido es un pseudo-postorden, porque hacemos IDR (postorden) o DIR (postorden modificado), aleatoriamente
void _auxEjecutarTarea(abb arbolJugadores, abb arbolRecorrido)
{
    int numJugs, jugMatar;
    tipoelem jugador;
    if (!es_vacio(arbolRecorrido))
    {
        if (_aleatorio(1, 2) == 1)
        {
            _auxEjecutarTarea(arbolJugadores, izq(arbolRecorrido));
            _auxEjecutarTarea(arbolJugadores, der(arbolRecorrido));
        }
        else
        {
            _auxEjecutarTarea(arbolJugadores, izq(arbolRecorrido));
            _auxEjecutarTarea(arbolJugadores, der(arbolRecorrido));
        }
        leer(arbolRecorrido, &jugador);
        if (jugador.rol == ROL_IMPOSTOR && !es_vacia_cola(jugador.tareas))
        { // Por cada impostor...
            numJugs = _numJugsPorHabitacion(arbolJugadores, ROL_TRIPULANTE, primero(jugador.tareas).lugarTarea);
            if (numJugs > 1)
            { // Miramos si hay algún tripulante en la habitación del impostor. Solo lo matamos si hay más de uno, porque si solo hay dos jugadores el impostor se delata
                if (_aleatorio(1, 3) != 1)
                { // Sucederá 2/3 de las veces, es decir, el impostor no siempre matará a alguien en su habitación
                    jugMatar = _aleatorio(1, numJugs);
                    _matarJugPorIndiceHabitacion(arbolJugadores, jugMatar, ROL_TRIPULANTE, primero(jugador.tareas).lugarTarea);
                }
            }
        }
    }
}

void _ejecutarTarea(abb A)
{
    _auxEjecutarTarea(A, A);
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

// Elimina la tarea del primero de la cola de tareas del todos los jugadores con tareas del árbol
void _siguienteTarea(abb A)
{
    tipoelem jugador;
    if (!es_vacio(A))
    {
        _siguienteTarea(izq(A));
        leer(A, &jugador);
        if (!es_vacia_cola(jugador.tareas))
        {
            suprimir_cola(&(jugador.tareas));
        }
        modificar(A, jugador);
        _siguienteTarea(der(A));
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
void leerArchivo(abb *A)
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
                printf("%-15s  |%-15s|\n", jugador.nombreJugador, primero(jugador.tareas).lugarTarea);
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

//Función que genera los datos de una partida: jugadores, roles y tareas
void jugar(abb *Arbol)
{
    int numJugadores, numImpostores, contador, victoria; // Variables auxiliares que usaremos para hacer la asignación
    char opcion;                                         // Para el menú
    tipoelem jugador;                                    // Variable auxiliar para almacenar un jugador (se reasignará según sea conveniente)
    abb arbolJuego;                                      // Aquí guardaremos los jugadores de la partida
    crear(&arbolJuego);                                  // Creamos el árbol de los jugadores de la partida
    char *nombreJugador = NULL;

    _limpiarDatos(*Arbol); // Limpiamos el árbol antes de generar una nueva partida

    do
    {
        printf("Número de participantes (4-10): ");
        scanf(" %d", &numJugadores);
    } while (!(numJugadores >= 4 && numJugadores <= 10));

    do
    {
        printf("Quieres hacer automáticamente el reparto de jugadores? (s/n): ");
        scanf(" %c", &opcion);
    } while (opcion != 's' && opcion != 'S' && opcion != 'n' && opcion != 'N');

    numImpostores = round(numJugadores / 5.0); // Calculamos el número de impostores. Los x primeros jugadores que entren en el árbol serán impostores (esto podemos hacerlo porque entran en orden aleatorio, así que es lo mismo que que repartamos los roles después de que estén en el árbol)

    if (opcion == 's' || opcion == 'S')
    { // Repartir los jugadores automáticamente

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
                contador++; // Insertamos el siguiente jugador
            }
        }
        //printf("\nJugadores seleccionados:\n");
        //_listadoJugadoresVivosYUltimasHabitaciones(arbolJuego);
    }
    else
    { // Repartir los jugadores manualmente. No se admite en esta versión de la práctica.
        contador = 0;
        while (contador < numJugadores)
        {
            nombreJugador = (char *)malloc(sizeof(char) * (L_NOMBRE + 1)); // Reservamos memoria para guardar el nombre del jugador
            printf("Nombre de jugador: ");
            scanf(" %s", nombreJugador);
            if (nombreJugador[0] == '@')
            {
                buscar_nodo(*Arbol, nombreJugador, &jugador);
                if (es_miembro(*Arbol, jugador))
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
    }

    printf("\nNúmero de impostores: %d\n", numImpostores);

    //printf("\nEstado final de los jugadores:\n");
    //listadoJugadores(arbolJuego);

    nombreJugador = (char *)malloc(sizeof(char) * (L_NOMBRE + 1));
    do
    {
        printf("\nJugadores        |Ultima habitac.|\n----------------------------------\n");
        _listadoJugadoresVivosYUltimasHabitaciones(arbolJuego);
        printf("\n");
        _ejecutarTarea(arbolJuego);

        printf("\nQuien es el impostor? ");
        scanf(" %s", nombreJugador); // Le pedimos un nombre de jugador para eliminar al usuario, si no es un nombre válido se supone que queremos pasar el turno
        if (nombreJugador[0] == '@')
        {
            buscar_nodo(arbolJuego, nombreJugador, &jugador);
            if (strcmp(nombreJugador, jugador.nombreJugador) == 0)
            {                                                                     // Si hemos encontrado el jugador...
                if (jugador.rol == ROL_IMPOSTOR || jugador.rol == ROL_TRIPULANTE) // El jugador tiene que estar vivo
                {
                    _matarJugador(jugador, arbolJuego);
                } else {
                    printf("Ese jugador ya está muerto. No se ha matado a nadie.\n");
                }
            }
        }
        else
        {
            printf("No se ha matado a nadie.\n");
        }

        _siguienteTarea(arbolJuego);
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

void guardarArchivo(abb A)
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
