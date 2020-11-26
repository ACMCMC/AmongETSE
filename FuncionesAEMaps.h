#ifndef FUNCIONESAEMAPS_H
#define FUNCIONESAEMAPS_H

#include "grafo.h"
#include "ConstantesAmongETSE.h"

//FUNCIONES DEL PROGRAMA DE PRUEBA DE GRAFOS
//Opción a del menú, introducir un vertice en el grafo
void introducir_vertice(grafo *G);

//Opción b del menú, eliminar un vértice del grafo
void eliminar_vertice(grafo *G);

//Opción c del menú, crear una relación entre dos vértices
void nuevo_arco(grafo *G);

//Opción d del menú, eliminar una relación entre dos vértices
void eliminar_arco(grafo *G);

//Opción i del menú, imprimir el grafo
void imprimir_grafo(grafo G);

//Se ejecuta al principio del programa, rellena el grafo con los datos de un archivo
void leerArchivoGrafo(grafo *G);

//Opción f del menú, guarda el grafo en un archivo
void guardarArchivoGrafo(grafo G);

//Calcula la ruta más corta entre dos vértices usando el algoritmo de Floyd-Warshall
void rutaMasCorta(grafo G);

//Calcula el árbol de expansión de coste mínimo usando el algoritmo de Prim
void arbolExpansion(grafo G);


#endif	/* FUNCIONESAEMAPS_H */

