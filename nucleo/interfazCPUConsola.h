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
				WAIT, SIGNAL, ENTRADA_SALIDA, CERRAR_CPU, PROGRAMA_TERMINADO} interfazPropia;

int serializarCargarPCB(t_PCB* pcb, void** serializacion);

int serializarCancelar(void** serializacion);

int serializarTerminar(void** serializacion);

int serializarImprimir(char* mensaje, void** serializacion); //para consola

int serializarContinuarEjecucion(void** serializacion);

int serializarDesalojar(void** serializacion);

void deserializarImprimir(void* parametrosSerializados, void* cpu);

void deserializarQuantumTerminado(void* parametrosSerializados, void* cpu);

void deserializarObtenerValor(void* parametrosSerializados, void* cpu);

void deserializarGrabarValor(void* parametrosSerializados, void* cpu);

void deserializarWait(void* parametrosSerializados, void* cpu);

void deserializarSignal(void* parametrosSerializados, void* cpu);

void deserializarEntradaSalida(void* parametrosSerializados, void* cpu);

void deserializarCerrarCPU(void* parametrosSerializados, void* cpu);

void deserializarProgramaTerminado(void* parametrosSerializados, void* cpu);

extern int quantum;

extern void imprimir(char* mensaje, void* cpu);

extern void quantumTerminado(void* cpu);

extern void obtener_valor(char* identificador, void* cpu);

extern void grabar_valor(char* identificador, int valorAGrabar);

extern void esperar(char* identificador, void* cpu);

extern void avisar(char* identificador);

extern void entradaSalida(char* identificador, int operaciones, void* cpu);

extern void cerrarCPU(void* cpu, void* ultimoMensaje);

extern void programaTerminado(void* cpu);

void (*deserializadores[9])(void*, void*);

void procesarMensaje(void* mensaje, void* dataAdicional);

extern t_log* logger;

#endif /* INTERFAZCPUCONSOLA_H_ */
