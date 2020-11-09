#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grafo.h"
#include "ConstantesAmongETSE.h"

//FUNCIONES DEL PROGRAMA DE PRUEBA DE GRAFOS

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
        if (!son_adyacentes_T(*G, posicion(*G, v1), posicion(*G, v2)))
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
        if (!son_adyacentes_I(*G, posicion(*G, v1), posicion(*G, v2)))
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
    if (son_adyacentes_T(*G, posicion(*G, v1), posicion(*G, v2)))
        borrar_arco_T(G, posicion(*G, v1), posicion(*G, v2));
    if (son_adyacentes_I(*G, posicion(*G, v1), posicion(*G, v2)))
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
    printf("El grafo actual es:\n");
    for (i = 0; i < N; i++)
    {
        //Imprimo el vértice
        printf("Vertice(%d): %s\n", i, VECTOR[i].habitacion);
        //Chequeo sus arcos
        for (j = 0; j < N; j++)
            if (son_adyacentes_I(G, i, j))
            { // Si dos vértices son adyacentes en la matriz de adyacencia de impostores, entonces tendrán menos distancia que en la de adyacencia de tripulantes (por la semántica del proyecto). Por eso, si un arco existe en esta matriz de adyacencia, no lo imprimimos otra vez si existe en la de tripulantes.
                printf("\t%s···%s\n", VECTOR[i].habitacion, VECTOR[j].habitacion);
            }
            else if (son_adyacentes_T(G, i, j))
            {
                printf("\t%s---%s\n", VECTOR[i].habitacion, VECTOR[j].habitacion);
            }
    }
}

// Opción f del menú, guarda el grafo en un archivo
void leerArchivoGrafo(grafo *G)
{
    tipovertice vertice, v1, v2;
    FILE *fp;
    fp = fopen("grafo.txt", "r"); // Abrimos el archivo en modo lectura
    char codigos[3]; // Vale 3 porque tenemos dos caracteres, y un '\0'
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
        for (i = 0; i < num_vertices(G); i++) {
            fprintf(fp, "%s\n", VECTOR[i].habitacion);
        }
        fprintf(fp, "*\n");
        for (i = 0; i < num_vertices(G); i++)
        {
            //Chequeo sus arcos
            for (j = 0; j < i; j++)
                if (son_adyacentes_I(G, i, j))
                { // Si dos vértices son adyacentes en la matriz de adyacencia de impostores, entonces tendrán menos distancia que en la de adyacencia de tripulantes (por la semántica del proyecto). Por eso, si un arco existe en esta matriz de adyacencia, no lo imprimimos otra vez si existe en la de tripulantes.
                    fprintf(fp, "%s..%d-%s\n", VECTOR[i].habitacion, son_adyacentes_I(G, i, j), VECTOR[j].habitacion);
                }
                else if (son_adyacentes_T(G, i, j))
                {
                    fprintf(fp, "%s--%d-%s\n", VECTOR[i].habitacion, son_adyacentes_T(G, i, j), VECTOR[j].habitacion);
                }
        }
        fclose(fp); // Cerramos el archivo, no necesitamos liberar memoria
    }
}
