/*
 * interfazCPU.h
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#ifndef INTERFAZCPUCONSOLA_H_
#define INTERFAZCPUCONSOLA_H_

#include <stdlib.h>
#include "pcb.h"

typedef enum {CARGAR_PCB, TERMINAR /*, CANCELAR*/} interfazCPU;

typedef enum{IMPRIMIR /*, TERMINAR*/ } interfazConsola;

typedef enum {/*IMPRIMIR, */ QUANTUM_TERMINADO = 1, CANCELAR = 2} interfazPropia;

int serializarCargarPCB(t_PCB* pcb, int quantum, void** serializacion);

int serializarCancelar(void** serializacion);

int serializarTerminar(void** serializacion);

int serializarImprimir(char* mensaje, void** serializacion); //para consola

void deserializarImprimir(void* parametrosSerializados, void* dataAdicional);

void deserializarQuantumTerminado(void* parametrosSerializados, void* dataAdicional);

void deserializarCancelar(void* parametrosSerializados, void* dataAdicional);

extern void imprimir(int pid, char* mensaje);

extern void quantumTerminado(t_PCB* pcbActualizado);

extern void cancelar(void* consola);

void (*deserializadores[3])(void*, void*);

void procesarMensaje(void* mensaje, void* dataAdicional);

#endif /* INTERFAZCPUCONSOLA_H_ */
