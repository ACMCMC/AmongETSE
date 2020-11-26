#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "grafo.h"
#include "ConstantesAmongETSE.h"

struct camino // Struct que utilizamos en _floyd, para guardar información del vértice previo y el mapa al que pertenece
{
    int verticePrevio;
    char mapa; // 'I' o 'T'
};

void _printMatrix(int matrix[][MAXVERTICES], int V)
{
    int i, j;
    printf("\n");
    for (i = 0; i < V; i++)
    {
        for (j = 0; j < V; j++)
        {
            if (matrix[i][j] == INFINITY)
                printf("%4s", "INF");
            else
                printf("%4d", matrix[i][j]);
        }
        printf("\n");
    }
}

// Imprime la matriz de vértice previo
void _printPrevVertex(struct camino matrix[][MAXVERTICES], int V)
{
    int i, j;
    printf("\n");
    for (i = 0; i < V; i++)
    {
        for (j = 0; j < V; j++)
        {
            printf("%4d", matrix[i][j].verticePrevio);
        }
        printf("\n");
    }
}

void _prim(grafo G, char mapa)
{
    int N;                  // El número de vértices del grafo
    int *selected;          // Este vector va a guardar la lista de vértices seleccionados (0 = no seleccionado, 1 = seleccionado)
    int i, j;               // Variables auxiliares
    int numArcos = 0;       // El número de arcos del árbol de expansión
    int distanciaTotal = 0; // El coste total del árbol
    int minimo;             // Variable auxiliar que en cada paso guardará el arco de mínimo coste que podemos añadir al árbol, sin que forme un ciclo
    int vx, vy;             // Los vértices que estamos cogiendo para mirar si añadir su arco

    //////////////////////////////////////////////////////////////////
    //
    // Inicialización de conjunto de vértices seleccionados
    //
    //////////////////////////////////////////////////////////////////

    N = num_vertices(G);                       // Asignamos valor a N
    selected = (int *)malloc(sizeof(int) * N); // Tenemos que reservar dinámicamente el vector, porque no conocemos a priori su tamaño (o podríamos usar también MAXVERTICES)

    for (i = 1; i < N; i++) // Inicializamos a 0 (no seleccionado) el vector de vértices seleccionados
    {
        selected[i] = 0;
    }
    selected[0] = 1; // El primer elemento del vector de vértices seleccionados siempre estará seleccionado, esta es una decisión arbitraria porque necesitamos un vértice para empezar el algoritmo de Prim

    //////////////////////////////////////////////////////////////////
    //
    // Cálculo del árbol de expansión de coste mínimo
    //
    //////////////////////////////////////////////////////////////////

    while (numArcos < N - 1) // Mientras no hayamos recorrido todos los vértices del grafo... (la condición es numArcos < N-1, porque el número de arcos de un grafo siempre es por lo menos una unidad menor que su número de vértices si éste es fuertemente conexo; puede tener más arcos pero como éste es el árbol de expansión de coste mínimo nos interesa parar cuando hayamos recorrido todos los vértices, para lo cual necesitamos tener N-1 arcos. Esta condición no obliga a que los arcos no formen ciclos, pero eso lo conseguimos con las comprobaciones del código de dentro de este lazo)
    {
        minimo = INFINITY;      // Empezamos con un coste mínimo de infinito, por lo que cualquier arco tendrá menos coste
        for (i = 0; i < N; i++) // Recorremos todo el vector de vértices seleccionados
        {
            if (selected[i] == 1) // Si el vértice que miramos ha sido seleccionado...
            {
                for (j = 0; j < N; j++) // Volvemos a recorrer el vector mirando qué arcos están conectados con ese vértice
                {
                    switch (mapa)
                    {
                    case 'T':                                         // Si estamos operando con el mapa de tripulantes...
                        if (selected[j] != 1 && distancia_T(G, i, j)) // Si el vértice j que estamos mirando ahora no ha sido seleccionado (crearía un ciclo), y está conectado con el vértice i, entonces...
                        {
                            if (minimo > distancia_T(G, i, j)) // Miramos si la distancia entre los vértices i y j es menor que la que hemos guardado en el mínimo
                            {
                                minimo = distancia_T(G, i, j); // Si es menor, nos quedamos con esta distancia, para obtener el arco con menor coste a nivel global (optimizamos el coste; es un algoritmo voraz)
                                vx = i;
                                vy = j;
                            }
                        }

                        break;
                    case 'I': // Lo mismo que arriba, pero para el mapa de impostores
                        if (selected[j] != 1 && distancia_I(G, i, j))
                        {
                            if (minimo > distancia_I(G, i, j))
                            {
                                minimo = distancia_I(G, i, j);
                                vx = i;
                                vy = j;
                            }
                        }

                        break;
                    }
                }
            }
        }

        selected[vy] = 1;         // Una vez hemos recorrido todo el vector probando todas las posibles combinaciones de vectores, el vértice vy que hemos seleccionado será el que tenga menor coste para conectarlo a nuestro árbol de expansión. Lo marcamos como seleccionado.
        numArcos++;               // Hemos seleccionado un vértice más, así que el número de arcos se incrementa
        distanciaTotal += minimo; // La distancia total también se incrementa con el nuevo arco que estamos introduciendo

        // Imprimimos el vértice y el arco seleccionado
        if (mapa == 'I' && distancia_I(G, vx, vy) != distancia_T(G, vx, vy)) // Si estamos en el mapa de impostores, y la distancia entre los vértices seleccionados en este paso es distinta que la del mapa de tripulantes, el arco es una rejilla, y lo imprimimos como tal
        {
            printf("%15s ·· %-15s: " COLOR_CYAN "%d\n" COLOR_RESET, array_vertices(G)[vx].habitacion, array_vertices(G)[vy].habitacion, minimo);
        }
        else // El arco es del mapa de tripulantes
        {
            printf("%15s -- %-15s: " COLOR_CYAN "%d\n" COLOR_RESET, array_vertices(G)[vx].habitacion, array_vertices(G)[vy].habitacion, minimo);
        }
    }

    printf("Distancia Total del arbol de expansion de coste minimo=" COLOR_CYAN "%d\n" COLOR_RESET, distanciaTotal);

    free(selected); // Liberamos la memoria del vector
}

// Declaración de la función recursiva privada que imprime el camino para llegar de i a j
void _printPath(struct camino P[][MAXVERTICES], int i, int j, tipovertice *V, int N);

void _floyd(grafo G, int origen, int destino, char tipo)
{
    int matrizDistancias[MAXVERTICES][MAXVERTICES];
    struct camino matrizVPrevio[MAXVERTICES][MAXVERTICES];
    struct camino caminoActual; // Lo usaremos para introducir caminos en la matriz de vértice previo y cambiar sus valores
    int k, j, i;                // Variables auxiliares
    int N;                      // Número de vértices
    int distanciaEnMatriz;      // La distancia entre dos vértices i y j que tiene guardada nuestra matriz de distancias
    int distanciaNueva;         // La distancia que tendría pasando por un vértice intermedio k, lo usaremos para compararla con distanciaEnMatriz y quedarnos con la menor de las dos

    N = num_vertices(G); // N es el número de vértices del grafo, aunque nuestra matriz soporta MAXVERTICES vamos a inicializar sólo los N primeros, ya que el resto no nos importa

    //////////////////////////////////////////////////////////////////
    //
    // Inicialización de matrices
    //
    //////////////////////////////////////////////////////////////////

    // MATRIZ DE DISTANCIAS
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            matrizDistancias[i][j] = INFINITY; // Empezamos con una distancia en principio infinita, y si se cumple alguna de las otras condiciones, la actualizaremos

            switch (tipo)
            {
            case 'I': // Estamos trabajando con el grafo de impostores
                if (distancia_I(G, i, j))
                { // Si la distancia en la matriz de adyacencia != 0, la distancia en la matriz de distancias será esa distancia
                    matrizDistancias[i][j] = distancia_I(G, i, j);
                }
                break;
            case 'T': // Trabajamos con el grafo de tripulantes
                if (distancia_T(G, i, j))
                { // Si la distancia en la matriz de adyacencia != 0, la distancia en la matriz de distancias será esa distancia
                    matrizDistancias[i][j] = distancia_T(G, i, j);
                }
                break;
            }

            if (i == j)
            { // Si los índices son iguales, la distancia es 0
                matrizDistancias[i][j] = 0;
            }
        }
    }

    // MATRIZ DE VÉRTICE PREVIO
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            caminoActual.verticePrevio = 0; // Empezamos, en principio, sin vértice previo
            caminoActual.mapa = tipo;

            switch (tipo)
            {
            case 'I': // Estamos trabajando con el grafo de impostores
                if (distancia_I(G, i, j) && i != j)
                { // Si la distancia en la matriz de adyacencia != 0, la distancia en la matriz de distancias será esa distancia
                    caminoActual.verticePrevio = i;
                }

                if (distancia_I(G, i, j) == distancia_T(G, i, j))
                { // Si la distancia en la matriz de adyacencia de impostores es la misma que en la de tripulantes, es porque es el mismo arco del grafo, y en ese caso lo tomamos como si "perteneciese al mapa de tripulantes"
                    caminoActual.mapa = 'T';
                }
                break;
            case 'T': // Trabajamos con el grafo de tripulantes
                if (distancia_T(G, i, j) && i != j)
                { // Si la distancia en la matriz de adyacencia != 0, la distancia en la matriz de distancias será esa distancia
                    caminoActual.verticePrevio = i;
                }
                break;
            }

            matrizVPrevio[i][j] = caminoActual; // Escribimos el camino actual en la matriz
        }
    }

    // Se pueden descomentar estas líneas para ver que la matriz de vértice previo y la de distancias se están inicializando correctamente:
    //_printMatrix(matrizDistancias, N);
    //_printPrevVertex(matrizVPrevio, N);

    //////////////////////////////////////////////////////////////////
    //
    // Búsqueda de distancias
    //
    // Buscamos la distancia mínima de forma iterativa, empezando por
    // el primer vértice, hasta el último vértice. Las matrices
    // resultantes serán la de distancias y la de vértice previo.
    //
    //////////////////////////////////////////////////////////////////

    for (k = 0; k < N; k++) // Por cada vértice...
    {
        for (i = 0; i < N; i++)
        {
            for (j = 0; j < N; j++)
            {
                distanciaEnMatriz = matrizDistancias[i][j];                       // La distancia que tiene guardada actualmente la matriz de distancias entre i y j
                distanciaNueva = matrizDistancias[i][k] + matrizDistancias[k][j]; // La distancia que tendrían i y j, si el camino que los conectase pasara por k
                if (distanciaEnMatriz > distanciaNueva)                           // Si pasando por k tenemos un camino más corto, cambiamos el camino que teníamos guardado y lo cambiamos por el que pase por k
                {
                    matrizDistancias[i][j] = distanciaNueva;   // Actualizamos la distancia entre i y j
                    matrizVPrevio[i][j] = matrizVPrevio[k][j]; // El vértice previo a j para llegar de i a j será el vértice previo a j para llegar de k a j, así hacemos que nuestro camino pase por k y no por donde estuviera pasando antes
                }
            }
        }
    }

    // Se pueden descomentar estas líneas para ver que la matriz de vértice previo y la de distancias se están calculando correctamente:
    //_printMatrix(matrizDistancias, N);
    //_printPrevVertex(matrizVPrevio, N);

    //////////////////////////////////////////////////////////////////
    //
    // Imprimimos el camino
    //
    //////////////////////////////////////////////////////////////////

    printf("La distancia total desde " COLOR_CYAN "%s" COLOR_RESET " hasta " COLOR_CYAN "%s" COLOR_RESET " es de " COLOR_MAGENTA "%d" COLOR_RESET "\n", array_vertices(G)[origen].habitacion, array_vertices(G)[destino].habitacion, matrizDistancias[origen][destino]);
    printf("\tRuta:   ");
    _printPath(matrizVPrevio, origen, destino, array_vertices(G), N); // Imprimimos la ruta
}

// Función recursiva que imprime el camino para llegar de i a j. Va volviendo hacia atrás desde j a i, y cuando llega a i, imprime el camino
void _printPath(struct camino P[][MAXVERTICES], int i, int j, tipovertice *V, int N)
{
    if (i != j) // Si ésta no es la habitación de origen... (que lo sea es nuestra condición de parada de la función recursiva)
    {
        _printPath(P, i, P[i][j].verticePrevio, V, N);
        switch (P[i][j].mapa) // Imprimimos el tipo de arco, si no es la habitación de origen
        {
        case 'T':
            printf(" --");
            break;
        case 'I':
            printf(" ··");
            break;
        }
    }
    printf("%s", V[j].habitacion); // Imprimimos esta habitación
}

//Opción a del menú, introducir un vertice en el grafo
void introducir_vertice(grafo *G)
{
    tipovertice v1;
    printf("Introduce vertice: ");
    scanf("%s", v1.habitacion);
    if (existe_vertice(*G, v1))
        printf("Ese vertice ya esta en el grafo\n");
    else
        insertar_vertice(G, v1);
}

//Opción b del menú, eliminar un vértice del grafo
void eliminar_vertice(grafo *G)
{
    tipovertice v1;
    printf("Introduce vertice: ");
    scanf("%s", v1.habitacion);
    if (existe_vertice(*G, v1))
        borrar_vertice(G, v1);
    else
        printf("Ese vertice no existe en el grafo\n");
}

//Opción c del menú, crear una relación entre dos vértices
void nuevo_arco(grafo *G)
{
    tipovertice v1, v2;
    int distancia;
    char opcion;

    //Insertamos una nueva relación pidiendo los datos al usuario controlando que existan los vértices
    //Vértice origen del arco
    printf("Introduce habitación origen: ");
    scanf(" %s", v1.habitacion);
    if (!existe_vertice(*G, v1))
    {
        printf("El vertice %s no existe en el grafo\n", v1.habitacion);
        return;
    }

    //Vértice destino del arco
    printf("Introduce habitación destino: ");
    scanf(" %s", v2.habitacion);
    if (!existe_vertice(*G, v2))
    {
        printf("El vertice %s no existe en el grafo\n", v2.habitacion);
        return;
    }

    printf("Distancia: ");
    scanf(" %d", &distancia);
    if (distancia < 1)
    {
        printf("Distancia no válida\n");
        return;
    }

    printf("Comunicacion mapa tripulantes (T) o impostores (I)? ");
    scanf(" %c", &opcion);
    switch (opcion)
    {
    case 'T':
    case 't':
        if (!distancia_T(*G, posicion(*G, v1), posicion(*G, v2)))
        {
            //Creación del arco
            crear_arco_T(G, posicion(*G, v1), posicion(*G, v2), distancia);
        }
        else
        {
            printf("Ya existe un arco para esa combinación de vértices\n");
        }
        break;
    case 'I':
    case 'i':
        if (!distancia_I(*G, posicion(*G, v1), posicion(*G, v2)))
        {
            //Creación del arco
            crear_arco_I(G, posicion(*G, v1), posicion(*G, v2), distancia);
        }
        else
        {
            printf("Ya existe un arco para esa combinación de vértices\n");
        }
    break;
    default:
        printf("Opción no válida\n");
        break;
    }
}

//Opción d del menú, eliminar una relación entre dos vértices
void eliminar_arco(grafo *G)
{
    tipovertice v1, v2;
    //Eliminamos una relación pidiendo los datos al usuario controlando que existan los vértices
    printf("Eliminar relacion vertice1-->vertice2\n");
    //Vértice origen del arco
    printf("Introduce vertice origen: ");
    scanf("%s", v1.habitacion);
    if (!existe_vertice(*G, v1))
    {
        printf("El vertice %s no existe en el grafo\n", v1.habitacion);
        return;
    }
    //Vértice destino del arco
    printf("Introduce vertice destino: ");
    scanf("%s", v2.habitacion);
    if (!existe_vertice(*G, v2))
    {
        printf("El vertice %s no existe en el grafo\n", v2.habitacion);
        return;
    }
    //Eliminación del arco
    if (distancia_T(*G, posicion(*G, v1), posicion(*G, v2)))
        borrar_arco_T(G, posicion(*G, v1), posicion(*G, v2));
    if (distancia_I(*G, posicion(*G, v1), posicion(*G, v2)))
        borrar_arco_I(G, posicion(*G, v1), posicion(*G, v2));
}

//Opción i del menú, imprimir el grafo
//Función que imprime el grafo utilizando num_vertices para saber cuántos vértices tiene
//y array_vertices para recuperar el vector de vértices y recorrerlo

void imprimir_grafo(grafo G)
{
    tipovertice *VECTOR; //Para almacenar el vector de vértices del grafo
    int N;               //número de vértices del grafo

    //Para recorrerla, simplemente vamos a recorrer la matriz de adyacencia
    N = num_vertices(G);
    VECTOR = array_vertices(G);

    int i, j;
    printf("El mapa actual es:\n");
    for (i = 0; i < N; i++)
    {
        //Imprimo el vértice
        printf("Habitacion(%d): %s\n", i, VECTOR[i].habitacion);
        //Chequeo sus arcos
        for (j = 0; j < N; j++)
        {
            if (distancia_T(G, i, j))
            {
                printf(COLOR_GREEN "\t--" COLOR_RESET "%-25s(distancia=%d)\n", VECTOR[j].habitacion, distancia_T(G, i, j));
            }
            if (distancia_I(G, i, j) && (distancia_I(G, i, j) != distancia_T(G, i, j)))
            { // La información de la matriz de adyacencia de impostores duplica la de tripulantes, excepto donde hay rejillas, en cuyo caso la distancia será menor que en la matriz de tripulantes (distancia=1). Por eso solo nos interesa imprimir esta distancia si no está ya en la matriz de tripulantes (lo otro sería repetirse innecesariamente).
                printf(COLOR_RED "\t··" COLOR_RESET "%-25s(distancia=%d)\n", VECTOR[j].habitacion, distancia_I(G, i, j));
            }
        }
    }
}

// Opción f del menú, guarda el grafo en un archivo
void leerArchivoGrafo(grafo *G)
{
    tipovertice vertice, v1, v2;
    FILE *fp;
    fp = fopen("grafocompleto.txt", "r"); // Abrimos el archivo en modo lectura
    char codigos[3];                      // Vale 3 porque tenemos dos caracteres, y un '\0'
    int valor;
    if (fp)
    {
        while (fscanf(fp, "%s", vertice.habitacion) != EOF && vertice.habitacion[0] != '\0' && vertice.habitacion[0] != '*' && vertice.habitacion[1] != '\0')
        {
            insertar_vertice(G, vertice);
        }
        while (!feof(fp))
        {
            fscanf(fp, " %[^-.]%[-.]%d-%s", v1.habitacion, codigos, &valor, v2.habitacion); // Leemos una línea de las conexiones del archivo
            if (existe_vertice(*G, v1) && existe_vertice(*G, v2))
            {
                if (strcmp(codigos, "..") == 0)
                {                                                               // Si codigos es ".."...
                    crear_arco_I(G, posicion(*G, v1), posicion(*G, v2), valor); // Creamos un arco en la matriz de adyacencia de impostores
                }
                else if (strcmp(codigos, "--") == 0)
                {                                                               // Si codigos es "--"...
                    crear_arco_T(G, posicion(*G, v1), posicion(*G, v2), valor); // Creamos un arco en la matriz de adyacencia de tripulantes
                }
                else
                {
                    fprintf(stderr, "ERROR. Código no esperado leyendo el archivo de datos del grafo. Los códigos son: %s.\n", codigos);
                }
            }
            else
            {
                fprintf(stderr, "ERROR leyendo el archivo. El vértice %s no existe.\n", !existe_vertice(*G, v1) ? v1.habitacion : v2.habitacion);
            }
        }
        fclose(fp); // Cerramos el archivo, no es necesario liberar memoria
    }
}

void guardarArchivoGrafo(grafo G)
{
    int i, j;
    tipovertice *VECTOR;
    FILE *fp;
    fp = fopen("grafocompleto.txt", "w"); // Abrimos el archivo en modo escritura
    if (fp)
    {
        VECTOR = array_vertices(G);
        for (i = 0; i < num_vertices(G); i++)
        {
            fprintf(fp, "%s\n", VECTOR[i].habitacion);
        }
        fprintf(fp, "*\n");
        for (i = 0; i < num_vertices(G); i++)
        {
            //Chequeo sus arcos
            for (j = 0; j < i; j++)
            {
                if (distancia_T(G, i, j)) // Si existe una distancia entre los dos vértices en la matriz de tripulantes, la imprimimos al archivo.
                {
                    fprintf(fp, "%s--%d-%s\n", VECTOR[i].habitacion, distancia_T(G, i, j), VECTOR[j].habitacion);
                }
                if (distancia_I(G, i, j) && (distancia_I(G, i, j) != distancia_T(G, i, j)))
                { // La distancia en la matriz de impostores solo nos interesa imprimirla si no está duplicada en la de tripulantes; si ya la hemos impreso en la de tripulantes, entonces sería imprimir información redundante.
                    fprintf(fp, "%s..%d-%s\n", VECTOR[i].habitacion, distancia_I(G, i, j), VECTOR[j].habitacion);
                }
            }
        }
        fclose(fp); // Cerramos el archivo, no necesitamos liberar memoria
    }
}

//Calcula la ruta más corta entre dos vértices usando el algoritmo de Floyd-Warshall
void rutaMasCorta(grafo G)
{
    tipovertice habOrigen, habDestino;
    char rol;

    printf("Introduce la habitacion origen: ");
    scanf(" %s", habOrigen.habitacion);
    printf("Introduce la habitacion destino: ");
    scanf(" %s", habDestino.habitacion);
    if (!existe_vertice(G, habOrigen) || !existe_vertice(G, habDestino)) // Comprobamos si existen ambas habitaciones
    {
        printf("La combinación de habitaciones seleccionada no es válida.");
        return;
    }

    printf("Introduce el rol (" COLOR_RED "I" COLOR_RESET "/" COLOR_GREEN "T" COLOR_RESET "): ");
    scanf(" %c", &rol);

    rol = toupper(rol); // Si el carácter del rol (I o T) está en minúsculas, lo convertimos a mayúsculas

    if (rol != 'I' && rol != 'T')
    {
        printf("El rol seleccionado no es válido.");
        return;
    }

    // Invocamos al algoritmo de Floyd-Warshall
    _floyd(G, posicion(G, habOrigen), posicion(G, habDestino), rol);
}

//Calcula el árbol de expansión de coste mínimo usando el algoritmo de Prim
void arbolExpansion(grafo G)
{
    char rol;

    printf("Introduce el rol (" COLOR_RED "I" COLOR_RESET "/" COLOR_GREEN "T" COLOR_RESET "): ");
    scanf(" %c", &rol);

    rol = toupper(rol); // Si el usuario escribió su opción en minúsculas, la pasamos a mayúsculas

    if (rol != 'I' && rol != 'T')
    {
        printf("El rol seleccionado no es válido.");
        return;
    }

    _prim(G, rol);
}
