/*
 * interfazCPU.c
 *
 *  Created on: 7/6/2016
 *      Author: utnso
 */

#include "interfazNucleo.h"

int serializarImprimir(char* mensaje, void** serializacion){
	int tamanioMensaje = sizeof(char) * (string_length(mensaje) + 1);
	int tamanio = sizeof(interfazNucleo) + sizeof(int) + tamanioMensaje;
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	memset(aux, '\0', tamanio);

	*((interfazNucleo*) aux) = IMPRIMIR;
	aux += sizeof(interfazNucleo);

	*((int*) aux) = pcb_actual->pid;
	aux += sizeof(int);

	memcpy(aux, mensaje, tamanioMensaje);

	return tamanio;
}

int serializarQuantumTerminado(t_PCB* pcbActualizado, void** serializacion){
	int tamanio = sizeof(interfazNucleo) + calcularTamanioPCB(pcbActualizado);
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	*(interfazNucleo*)aux = QUANTUM_TERMINADO;
	aux += sizeof(interfazNucleo);

	serializarPCB(pcbActualizado, aux);

	return tamanio;
}

void deserializarCargarPCB(void* parametrosSerializados, void* dataAdicional){
	t_PCB* pcb = deserializarPCB(parametrosSerializados);

	cargarPCB(pcb);
}

void (*deserializadores[4])(void*, void*) = {terminar, cancelar, deserializarCargarPCB, ejecutarInstruccion};

void procesarMensaje(void* mensaje, void* dataAdicional){
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux, dataAdicional);
	free(mensaje);
}
