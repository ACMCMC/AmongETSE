#ifndef COLA_H
#define COLA_H

#include "ConstantesAmongETSE.h"

//////////////////////// ESTRUCTURAS DE DATOS

//Interfaz TAD cola

typedef struct tipocola *cola; /*tipo opaco*/

//////////////////////////////////////////INICIO PARTE MODIFICABLE
// Tipo de elemento de la cola. 
typedef struct {
    char lugarTarea[L_HABITACION];
    char descripcionTarea[L_DESC_TAREA];
} tipoelemCola;
//////////////////////////////////////////FIN PARTE MODIFICABLE

///////////////////////// FUNCIONES

/**
 * Fija a null el puntero
 * @param C 
 */
void crear_cola(cola *C);

/**
 * Libera la memoria de cada celda de la cola
 * @param C
 */
void destruir_cola(cola *C);

/**
 * Comprueba si la cola está vacía
 * @return 1 si la cola está vacía y 0 en otro caso
 */
unsigned es_vacia_cola(cola C);

/**
 * Devuelve el primer elemento de la cola. No comprueba 
 * si la cola está vacía, es posible que haya que llamar
 * a es_vacia_cola antes de llamar a estar función.
 * @param C
 * @return Primer elemento de la cola
 */
tipoelemCola primero(cola C);

/**
 * Introduce un nuevo elemento al final de la cola.
 * @param C
 * @param E 
 */
void insertar_cola(cola *C, tipoelemCola E);

/**
 * Elimina de la cola el primer elemento. No comprueba 
 * si la cola está vacía, es posible que haya que llamar
 * a es_vacia_cola antes de llamar a estar función.
 * @param C
 */
void suprimir_cola(cola *C);

#endif	// COLA_H


