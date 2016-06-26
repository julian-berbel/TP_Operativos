/*
 * interfazCPU.c
 *
 *  Created on: 7/6/2016
 *      Author: utnso
 */

#include "interfazCPUConsola.h"

int serializarCargarPCB(t_PCB* pcb, void** serializacion){
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

int serializarContinuarEjecucion(void** serializacion){
	int tamanio = sizeof(interfazCPU) + sizeof(int);
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	*((interfazCPU*) aux) = CONTINUAR_EJECUCION;
	aux += sizeof(interfazCPU);

	*((int*) aux) = quantum;

	return tamanio;
}

int serializarDesalojar(void** serializacion){
	int tamanio = sizeof(interfazCPU);
	*serializacion = malloc(tamanio);

	*((interfazCPU*) serializacion) = DESALOJAR;

	return tamanio;
}

void deserializarCancelar(void* parametrosSerializados, void* consola){
	cancelar(consola);
}

void deserializarImprimir(void* parametrosSerializados, void* cpu){
	char* mensaje = parametrosSerializados;

	imprimir(mensaje, cpu);
}

void deserializarQuantumTerminado(void* parametrosSerializados, void* cpu){
	quantumTerminado(cpu);
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

void (*deserializadores[7])(void*, void*) = {	deserializarImprimir, deserializarQuantumTerminado, deserializarObtenerValor,
												deserializarGrabarValor, deserializarWait, deserializarSignal, deserializarEntradaSalida};

void procesarMensaje(void* mensaje, void* dataAdicional){
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux, dataAdicional);
	free(mensaje);
}
