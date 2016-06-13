/*
 * interfazNucleo.h
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#ifndef INTERFAZNUCLEO_H_
#define INTERFAZNUCLEO_H_

#include <stdlib.h>
#include "pcb.h"

typedef enum {IMPRIMIR, QUANTUM_TERMINADO, CANCELAR} interfazNucleo;

typedef enum {CARGAR_PCB, TERMINAR /*, CANCELAR*/} interfazPropia;

int serializarImprimir(char* mensaje, void** serializacion);

int serializarQuantumTerminado(t_PCB* pcbActualizado, void** serializacion);

void deserializarCargarPCB(void* parametrosSerializados, void* dataAdicional);

void (*deserializadores[3])(void*, void*);

void procesarMensaje(void* mensaje, void* dataAdicional);

extern void cargarPCB(t_PCB* pcb, int quantum);

extern void cancelar();

extern void terminar();

extern t_PCB* pcb_actual;

#endif /* INTERFAZNUCLEO_H_ */
