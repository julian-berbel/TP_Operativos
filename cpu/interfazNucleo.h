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

typedef enum {CANCELAR = 1, IMPRIMIR = 2, QUANTUM_TERMINADO = 3} interfazNucleo;

typedef enum {TERMINAR, /*, CANCELAR*/ CARGAR_PCB, EJECUTAR_INSTRUCCION} interfazPropia;

int serializarImprimir(char* mensaje, void** serializacion);

int serializarQuantumTerminado(t_PCB* pcbActualizado, void** serializacion);

void deserializarCargarPCB(void* parametrosSerializados, void* dataAdicional);

void procesarMensaje(void* mensaje, void* dataAdicional);

extern void cargarPCB(t_PCB* pcb);

extern void ejecutarInstruccion();

extern void cancelar();

extern void terminar();

extern t_PCB* pcb_actual;

#endif /* INTERFAZNUCLEO_H_ */
