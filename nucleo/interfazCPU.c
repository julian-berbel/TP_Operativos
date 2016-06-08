/*
 * interfazCPU.c
 *
 *  Created on: 7/6/2016
 *      Author: utnso
 */

#include "interfazCPU.h"

int serializarCargarPCB(t_PCB* pcb, void** serializacion){
	int tamanio = sizeof(interfazCPU) + calcularTamanioPCB(pcb);
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	*((interfazCPU*) aux) = CARGAR_PCB;
	aux += sizeof(interfazCPU);

	serializarPCB(pcb, aux);

	return tamanio;
}

int serializarCancelar(void** serializacion) {
	*serializacion = malloc(sizeof(interfazCPU));
	*(interfazCPU*)*serializacion = CANCELAR;

	return sizeof(interfazCPU);
}

int serializarTerminar(void** serializacion) {
	*serializacion = malloc(sizeof(interfazCPU));
	*(interfazCPU*)*serializacion = TERMINAR;

	return sizeof(interfazCPU);
}

int serializarEjecutarInstruccion(void** serializacion) {
	*serializacion = malloc(sizeof(interfazCPU));
	*(interfazCPU*)*serializacion = EJECUTAR_INSTRUCCION;

	return sizeof(interfazCPU);
}

void deserializarCancelar(void* parametrosSerializados){
	int socket = *((int*) parametrosSerializados);

	cancelar(socket);
}

void deserializarImprimir(void* parametrosSerializados){
	int pid = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);
	char* mensaje = parametrosSerializados;

	imprimir(pid, mensaje);
}

void deserializarQuantumTerminado(void* parametrosSerializados){
	t_PCB* pcb = deserializarPCB(parametrosSerializados);

	quantumTerminado(pcb);
}

void (*deserializadores[3])(void*) = {deserializarCancelar, deserializarImprimir, deserializarQuantumTerminado};

void procesarMensaje(void* mensaje){
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux);
	free(mensaje);
}
