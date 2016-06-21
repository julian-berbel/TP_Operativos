/*
 * interfazCPU.c
 *
 *  Created on: 7/6/2016
 *      Author: utnso
 */

#include "interfazCPUConsola.h"

int serializarCargarPCB(t_PCB* pcb, int quantum, void** serializacion){
	int tamanio = sizeof(interfazCPU) + calcularTamanioPCB(pcb) + sizeof(int);
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	*((interfazCPU*) aux) = CARGAR_PCB;
	aux += sizeof(interfazCPU);

	*((int*) aux) = quantum;
	aux += sizeof(int);

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

int serializarImprimir(char* mensaje, void** serializacion){
	int tamanioMensaje = sizeof(char) * (string_length(mensaje) + 1);
	int tamanio = sizeof(interfazConsola) + tamanioMensaje;
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	memset(aux, '\0', tamanio);

	*((interfazConsola*) aux) = IMPRIMIR;
	aux += sizeof(interfazConsola);

	memcpy(aux, mensaje, tamanioMensaje);

	return tamanio;
}

void deserializarCancelar(void* parametrosSerializados, void* consola){
	cancelar(consola);
}

void deserializarImprimir(void* parametrosSerializados, void* cpu){
	int pid = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);
	char* mensaje = parametrosSerializados;

	imprimir(pid, mensaje);
}

void deserializarQuantumTerminado(void* parametrosSerializados, void* cpu){
	t_PCB* pcb = deserializarPCB(parametrosSerializados);

	quantumTerminado(pcb);
}

void deserializarObtenerValor(void* parametrosSerializados, void* cpu){
	char* identificador = parametrosSerializados;

	obtener_valor(identificador, cpu);
}

void deserializarGrabarValor(void* parametrosSerializados, void* cpu){
	int valorAGrabar = *((int*)parametrosSerializados);
	parametrosSerializados += sizeof(int);
	char* identificador = parametrosSerializados;

	grabar_valor(identificador, valorAGrabar);
}

void deserializarWait(void* parametrosSerializados, void* cpu){
	char* identificador = parametrosSerializados;
	esperar(identificador, cpu);
}

void deserializarSignal(void* parametrosSerializados, void* cpu){
	char* identificador = parametrosSerializados;
	avisar(identificador);
}

void deserializarEntradaSalida(void* parametrosSerializados, void* cpu){
	int operaciones = *((int*)parametrosSerializados);
	parametrosSerializados += sizeof(int);
	char* identificador = parametrosSerializados;

	entradaSalida(identificador, operaciones, cpu);
}

void (*deserializadores[8])(void*, void*) = {	deserializarImprimir, deserializarQuantumTerminado, deserializarCancelar, deserializarObtenerValor,
												deserializarGrabarValor, deserializarWait, deserializarSignal, deserializarEntradaSalida};

void procesarMensaje(void* mensaje, void* dataAdicional){
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux, dataAdicional);
	free(mensaje);
}
