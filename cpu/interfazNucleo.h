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
#include <commons/log.h>

typedef enum {IMPRIMIR, QUANTUM_TERMINADO, OBTENER_VALOR, GRABAR_VALOR, WAIT, SIGNAL, ENTRADA_SALIDA, CERRAR_CPU, PROGRAMA_TERMINADO} interfazNucleo;

typedef enum {CARGAR_PCB, TERMINAR} interfazPropia;

int serializarImprimir(char* mensaje, void** serializacion);

int serializarQuantumTerminado(void** serializacion);

int serializarObtenerValor(char* identificador, void** serializacion);

int serializarGrabarValor(char* identificador, int valor, void** serializacion);

int serializarWait(char* identificador, void** serializacion);

int serializarSignal(char* identificador, void** serializacion);

int serializarEntradaSalida(char* identificador, int operaciones, void** serializacion);

int serializarCerrarCPU(void** serializacion);

int serializarProgramaTerminado(void** serializacion);

void deserializarCargarPCB(void* parametrosSerializados, void* dataAdicional);

void (*deserializadores[4])(void*, void*);

void procesarMensaje(void* mensaje, void* dataAdicional);

extern void cargarPCB(t_PCB* pcb, int quantum);

extern void terminar();

extern void continuarEjecucion(int quantum);

extern void desalojar();

extern t_PCB* pcb_actual;

extern t_log* logger;

#endif /* INTERFAZNUCLEO_H_ */
