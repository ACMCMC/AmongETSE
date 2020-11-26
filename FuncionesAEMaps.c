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
            if (matrix[i][j] == INFINITO)
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

// Calcula el algoritmo de Floyd-Warshall y devuelve su resultado en matrizDistancias y matrizVPrevio
void _floyd(grafo G, char tipo, int matrizDistancias[][MAXVERTICES], struct camino matrizVPrevio[][MAXVERTICES])
{
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
            matrizDistancias[i][j] = INFINITO; // Empezamos con una distancia en principio infinita, y si se cumple alguna de las otras condiciones, la actualizaremos

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

// Devuelve el vértice siguiente al origen en una ruta calculada por Floyd-Warshall
tipovertice verticeSiguiente(grafo G, tipovertice origen, tipovertice destino, char tipo)
{
    printf("ESTO VA 1\n");
    int j, siguienteJ; // Como vamos a recorrer el camino en sentido inverso (destino -> origen), queremos quedarnos con el vértice anterior al origen. Eso será siguienteJ, que siempre guarda el siguiente de lo que sea J en un momento determinado.
    int matrizDistancias[MAXVERTICES][MAXVERTICES];
    struct camino matrizVPrevio[MAXVERTICES][MAXVERTICES];
    if (strncmp(origen.habitacion, destino.habitacion, L_HABITACION)==0) {
        fprintf(stderr, "La habitación de origen es la de destino (%s -> %s).\n", origen.habitacion, destino.habitacion);
        //return origen;
    }
    if (existe_vertice(G, origen) && existe_vertice(G, destino))
    {
        _floyd(G, tipo, matrizDistancias, matrizVPrevio);
        j = posicion(G, destino); // Vamos a ir recorriendo la ruta en orden inverso hasta encontrar la siguiente habitación al origen
    printf("ESTO VA 2, %d\n", j);
        while (posicion(G, origen) != j)
        {
            siguienteJ = j;
    printf("ESTO VA 3, %d\n", siguienteJ);
            j = matrizVPrevio[posicion(G, origen)][j].verticePrevio;
        }
    }
    else
    {
        fprintf(stderr, "Error en verticeSiguiente: no existe la combinación especificada de vértices (%s -> %s).\n", origen.habitacion, destino.habitacion);
        siguienteJ = 0;
    }
    return array_vertices(G)[siguienteJ]; // No va a ser un resultado correcto si hubo error
}
