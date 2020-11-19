#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "grafo.h"
#include "ConstantesAmongETSE.h"

#define INFINITY 1000

//FUNCIONES DEL PROGRAMA DE PRUEBA DE GRAFOS

struct camino
{
    int verticePrevio;
    char mapa;
};

void _printPath(struct camino P[][MAXVERTICES], int i, int j, tipovertice *V, int N);

void _printMatrix(int matrix[][MAXVERTICES], int V){
   int i,j;
      printf("\n");
   for(i=0;i<V;i++){
      for(j=0;j<V;j++){
         if (matrix[i][j]==INFINITY) 
            printf("%4s","INF");
         else
            printf("%4d",matrix[i][j]);
      }
      printf("\n");
   }
}

void _printMatrixVPrevio(struct camino matrix[][MAXVERTICES], int V){
   int i,j;
      printf("\n");
   for(i=0;i<V;i++){
      for(j=0;j<V;j++){
            printf("%4d",matrix[i][j].verticePrevio);
      }
      printf("\n");
   }
}

void _floyd(grafo G, int origen, int destino, char tipo)
{
    //////////////////////////////////////////////////////////////////
    //
    // Inicialización de matrices
    //
    //////////////////////////////////////////////////////////////////

    int matrizDistancias[MAXVERTICES][MAXVERTICES];
    struct camino matrizVPrevio[MAXVERTICES][MAXVERTICES];
    struct camino caminoActual;
    int k, j, i, N;
    int distanciaEnMatriz;
    int distanciaNueva;

    tipo = toupper(tipo); // Si el carácter del tipo (I o T) está en minúsculas, lo convertimos a mayúsculas

    N = num_vertices(G); // N es el número de vértices del grafo, aunque nuestra matriz soporta MAXVERTICES vamos a inicializar sólo los N primeros, ya que el resto no nos importa

    // MATRIZ DE DISTANCIAS
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            matrizDistancias[i][j] = INFINITY; // Empezamos con una distancia en principio infinita, y si se cumple alguna de las otras condiciones, la actualizaremos

            if (tipo == 'I')
            { // Estamos trabajando con el grafo de impostores
                if (distancia_I(G, i, j))
                { // Si la distancia en la matriz de adyacencia != 0, la distancia en la matriz de distancias será esa distancia
                    matrizDistancias[i][j] = distancia_I(G, i, j);
                }
            }
            else
            { // Trabajamos con el grafo de tripulantes
                if (distancia_T(G, i, j))
                { // Si la distancia en la matriz de adyacencia != 0, la distancia en la matriz de distancias será esa distancia
                    matrizDistancias[i][j] = distancia_T(G, i, j);
                }
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

            if (tipo == 'I')
            { // Estamos trabajando con el grafo de impostores
                if (distancia_I(G, i, j) && i != j)
                { // Si la distancia en la matriz de adyacencia != 0, la distancia en la matriz de distancias será esa distancia
                    caminoActual.verticePrevio = i;
                }
            }
            else
            { // Trabajamos con el grafo de tripulantes
                if (distancia_T(G, i, j) && i != j)
                { // Si la distancia en la matriz de adyacencia != 0, la distancia en la matriz de distancias será esa distancia
                    caminoActual.verticePrevio = i;
                }
            }

            matrizVPrevio[i][j] = caminoActual; // Escribimos el camino actual en la matriz
        }
    }

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
                distanciaEnMatriz = matrizDistancias[i][j];
                distanciaNueva = matrizDistancias[i][k] + matrizDistancias[k][j];
                if (distanciaEnMatriz > distanciaNueva)
                {
                    matrizDistancias[i][j] = distanciaNueva;
                    matrizVPrevio[i][j] = matrizVPrevio[k][j];
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////
    //
    // Imprimimos el camino
    //
    //////////////////////////////////////////////////////////////////

    _printPath(matrizVPrevio, origen, destino, array_vertices(G), N);
}

void _printPath(struct camino P[][MAXVERTICES], int i, int j, tipovertice *V, int N)
{
    if (i != j) {
        _printPath(P, i, P[i][j].verticePrevio, V, N);
    }
    printf("%s\n", V[j].habitacion);
}

void _printSolution(int matrix[][MAXVERTICES], int N)
{
}

void _printPrevVertex(struct camino P[][MAXVERTICES], int N)
{
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
    if (opcion == 'T' || opcion == 't')
    {
        if (!distancia_T(*G, posicion(*G, v1), posicion(*G, v2)))
        {
            crear_arco_T(G, posicion(*G, v1), posicion(*G, v2), distancia);
        }
        else
        {
            printf("Ya existe un arco para esa combinación de vértices\n");
            return;
        }
    }
    else if (opcion == 'I' || opcion == 'i')
    {
        if (!distancia_I(*G, posicion(*G, v1), posicion(*G, v2)))
        {
            crear_arco_I(G, posicion(*G, v1), posicion(*G, v2), distancia);
        }
        else
        {
            printf("Ya existe un arco para esa combinación de vértices\n");
            return;
        }
    }
    else
    {
        printf("Opción no válida\n");
        return;
    }

    //Creación del arco
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
            if (distancia_I(G, i, j))
            { // Si dos vértices son adyacentes en la matriz de adyacencia de impostores, entonces tendrán menos distancia que en la de adyacencia de tripulantes (por la semántica del proyecto). Por eso, si un arco existe en esta matriz de adyacencia, no haría falta imprimirlo en la de tripulantes, pero en el ejemplo de ejecución sí que se imprime 2 veces, así que yo lo hago también.
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
    fp = fopen("grafo.txt", "w"); // Abrimos el archivo en modo escritura
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
                if (distancia_I(G, i, j))
                { // Si dos vértices son adyacentes en la matriz de adyacencia de impostores, entonces tendrán menos distancia que en la de adyacencia de tripulantes (por la semántica del proyecto). Por eso, si un arco existe en esta matriz de adyacencia, no lo imprimimos otra vez si existe en la de tripulantes.
                    fprintf(fp, "%s..%d-%s\n", VECTOR[i].habitacion, distancia_I(G, i, j), VECTOR[j].habitacion);
                }
                else if (distancia_T(G, i, j))
                {
                    fprintf(fp, "%s--%d-%s\n", VECTOR[i].habitacion, distancia_T(G, i, j), VECTOR[j].habitacion);
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
    if (!existe_vertice(G, habOrigen) || !existe_vertice(G, habDestino))
    {
        printf("La combinación de habitaciones seleccionada no es válida.");
        return;
    }
    printf("Introduce el rol (" COLOR_RED "I" COLOR_RESET "/" COLOR_GREEN "T" COLOR_RESET "): ");
    scanf(" %c", &rol);
    if (rol != 'I' && rol != 'i' && rol != 'T' && rol != 't')
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
}
