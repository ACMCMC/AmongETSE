
#ifndef FUNCIONESAMONGETSE_H
#define FUNCIONESAMONGETSE_H

#include "abb.h"
#include "ConstantesAmongETSE.h"

/**
 * lee un archivo de jugadores de un fichero
 * @param A
 */
void leerArchivo(abb *A);

/**
 * Inserta un nuevo jugador en el árbol
 * @param A
 */
void altaJugador(abb *A);

/**
 * Elimina un jugador en el árbol
 * @param A
 */
void bajaJugador(abb *A);

/**
 * Imprime en inorden (alfabético) los nodos del árbol
 * @param A
 */
void listadoJugadores(abb A);

/**
 * Genera una nueva partida, asignando valores a cada jugador
 * @param A
 */
void jugar(abb *A);

/**
 * Consulta los datos de un jugador
 * @param A
 */
void consultarTareaJugador(abb A);

/**
 * Consulta los jugadores que hay en una habitación
 * @param A
 */
void consultarPorHabitacion(abb A);

/**
 * Guarda los jugadores en jugadores.txt
 * @param A
 */
void guardarArchivo(abb A);

#endif // FUNCIONESAMONGETSE_H
