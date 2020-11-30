#include "cola.h"
#include <stdlib.h>
#include <stdio.h>

//////////////////////////////// ESTRUCTURAS DE DATOS

struct celdaCola
{
    tipoelemCola elemento;
    struct celdaCola *sig;
};

typedef struct celdaCola *puntero;

struct tipocola
{
    puntero frente;
    puntero final;
};

//////////////////////////////// FUNCIONES

void crear_cola(cola *C)
{
    *C = (cola)malloc(sizeof(struct tipocola));
    (*C)->frente = (puntero)malloc(sizeof(struct celdaCola));
    (*C)->final = (*C)->frente;
    (*C)->frente->sig = NULL;
}

void destruir_cola(cola *C)
{
    puntero p, r;
    p = (*C)->frente;
    while (p != NULL)
    {
        r = p;
        p = p->sig;
        free(r);
    }
    free(*C);
}

unsigned es_vacia_cola(cola C)
{
    return (C->frente == C->final);
}

tipoelemCola primero(cola C)
{
    return (C->frente)->sig->elemento;
}

void insertar_cola(cola *C, tipoelemCola E)
{
    ((*C)->final)->sig = (puntero)malloc(sizeof(struct celdaCola));
    (*C)->final = ((*C)->final)->sig;
    ((*C)->final)->elemento = E;
    ((*C)->final)->sig = NULL;
}

void suprimir_cola(cola *C)
{
    puntero p;
    p = (*C)->frente;
    (*C)->frente = (*C)->frente->sig;
    free(p);
}
