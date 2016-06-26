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

typedef enum {CARGAR_PCB/*, TERMINAR*/, CONTINUAR_EJECUCION, DESALOJAR} interfazCPU;

typedef enum{/*IMPRIMIR, */ TERMINAR} interfazConsola;

typedef enum {	IMPRIMIR, QUANTUM_TERMINADO, OBTENER_VALOR, GRABAR_VALOR,
				WAIT, SIGNAL, ENTRADA_SALIDA} interfazPropia;

int serializarCargarPCB(t_PCB* pcb, void** serializacion);

int serializarCancelar(void** serializacion);

int serializarTerminar(void** serializacion);

int serializarImprimir(char* mensaje, void** serializacion); //para consola

int serializarContinuarEjecucion(void** serializacion);

int serializarDesalojar(void** serializacion);

void deserializarImprimir(void* parametrosSerializados, void* dataAdicional);

void deserializarQuantumTerminado(void* parametrosSerializados, void* dataAdicional);

void deserializarCancelar(void* parametrosSerializados, void* dataAdicional);

extern int quantum;

extern void imprimir(char* mensaje, void* cpu);

extern void quantumTerminado(void* cpu);

extern void cancelar(void* consola);

extern void obtener_valor(char* identificador, void* cpu);

extern void grabar_valor(char* identificador, int valorAGrabar);

extern void esperar(char* identificador, void* cpu);

extern void avisar(char* identificador);

extern void entradaSalida(char* identificador, int operaciones, void* cpu);

void (*deserializadores[7])(void*, void*);

void procesarMensaje(void* mensaje, void* dataAdicional);

#endif /* INTERFAZCPUCONSOLA_H_ */
