#ifndef GRAFO_H

#define GRAFO_H

#define MAXVERTICES 100 /*maximo numero de nodos*/

#include "ConstantesAmongETSE.h"

/*
 * Implementación estática del TAD grafo con una matriz
 * de adjacencias con máximo número de vértices MAXVERTICES 
 */

/////////////////////////////////////////////////////////// TIPOS DE DATOS

//Información que se almacena en cada vértice
typedef struct
{
    char habitacion[L_HABITACION];
} tipovertice;

typedef struct tipografo *grafo;

//////////////////////////////////////////////////////////////// FUNCIONES

//Creación del grafo con 0 nodos
void crear_grafo(grafo *G);

//Devuelve la posición del vértice Vert en el vector VERTICES del grafo G
//Si devuelve -1 es porque no encontró el vértice
int posicion(grafo G, tipovertice Vert);

//Devuelve 1 si el grafo G existe y 0 en caso contrario
int existe(grafo G);

//Devuelve 1 si el vértice Vert existe en el grafo G
int existe_vertice(grafo G, tipovertice Vert);

//Inserta un vértice en el grafo
// Devuelve la posición en el que ha sido insertado el
// vértice o -1 si no se ha conseguido insertar el vértice
int insertar_vertice(grafo *G, tipovertice Vert);

//Borra un vértice del grafo
void borrar_vertice(grafo *G, tipovertice Vert);

//Crea el arco de relación entre VERTICES(pos1) y VERTICES(pos2)
void crear_arco_T(grafo *G, int pos1, int pos2, int valor);
void crear_arco_I(grafo *G, int pos1, int pos2, int valor);

//Borra el arco de relación entre VERTICES(pos1) y VERTICES(pos2)
void borrar_arco_T(grafo *G, int pos1, int pos2);
void borrar_arco_I(grafo *G, int pos1, int pos2);

//Devuelve la distancia entre VERTICES(pos1) y VERTICES(pos2), si son vértices adyacentes
int distancia_T(grafo G, int pos1, int pos2);
int distancia_I(grafo G, int pos1, int pos2);

//Destruye el grafo
void borrar_grafo(grafo *G);

//Devuelve el número de vértices del grafo G
int num_vertices(grafo G);

//Devuelve el vector de vértices VERTICES del grafo G
tipovertice *array_vertices(grafo G);

#endif /* GRAFO_H */
