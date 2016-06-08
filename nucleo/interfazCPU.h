/*
 * interfazCPU.h
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#ifndef INTERFAZCPU_H_
#define INTERFAZCPU_H_

#include <stdlib.h>
#include "pcb.h"

typedef enum {TERMINAR, CARGAR_PCB, EJECUTAR_INSTRUCCION /*, CANCELAR*/} interfazCPU;

typedef enum {/*TERMINAR, */ CANCELAR = 1, IMPRIMIR = 2, QUANTUM_TERMINADO = 3} interfazPropia;

int serializarCargarPCB(t_PCB* pcb, void** serializacion);

int serializarCancelar(void** serializacion);

int serializarTerminar(void** serializacion);

int serializarEjecutarInstruccion(void** serializacion);

void deserializarImprimir(void* parametrosSerializados);

void deserializarQuantumTerminado(void* parametrosSerializados);

void deserializarCancelar(void* parametrosSerializados);

extern void imprimir(int pid, char* mensaje);

extern void quantumTerminado(t_PCB* pcbActualizado);

extern void cancelar(int socketConsola);

void procesarMensaje(void* mensaje);

#endif /* INTERFAZCPU_H_ */
