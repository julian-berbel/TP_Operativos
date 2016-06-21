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

int serializarObtenerValor(char* identificador, void** serializacion){
	int tamanioIdentificador = sizeof(char) * (string_length(identificador) + 1);
	int tamanio = sizeof(interfazNucleo) + tamanioIdentificador;
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	memset(aux, '\0', tamanio);

	*((interfazNucleo*) aux) = OBTENER_VALOR;
	aux += sizeof(interfazNucleo);

	memcpy(aux, identificador, tamanioIdentificador);

	return tamanio;
}

int serializarGrabarValor(char* identificador, int valor, void** serializacion){
	int tamanioIdentificador = sizeof(char) * (string_length(identificador) + 1);
	int tamanio = sizeof(interfazNucleo) + sizeof(int) + tamanioIdentificador;
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	memset(aux, '\0', tamanio);

	*((interfazNucleo*) aux) = GRABAR_VALOR;
	aux += sizeof(interfazNucleo);

	*((int*)aux) = valor;
	aux += sizeof(int);

	memcpy(aux, identificador, tamanioIdentificador);

	return tamanio;
}

int serializarWait(char* identificador, void** serializacion){
	int tamanioIdentificador = sizeof(char) * (string_length(identificador) + 1);
	int tamanio = sizeof(interfazNucleo) + tamanioIdentificador;
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	memset(aux, '\0', tamanio);

	*((interfazNucleo*) aux) = WAIT;
	aux += sizeof(interfazNucleo);

	memcpy(aux, identificador, tamanioIdentificador);

	return tamanio;
}

int serializarSignal(char* identificador, void** serializacion){
	int tamanioIdentificador = sizeof(char) * (string_length(identificador) + 1);
	int tamanio = sizeof(interfazNucleo) + tamanioIdentificador;
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	memset(aux, '\0', tamanio);

	*((interfazNucleo*) aux) = SIGNAL;
	aux += sizeof(interfazNucleo);

	memcpy(aux, identificador, tamanioIdentificador);

	return tamanio;
}

int serializarEntradaSalida(char* identificador, int operaciones, void** serializacion){
	int tamanioIdentificador = sizeof(char) * (string_length(identificador) + 1);
	int tamanio = sizeof(interfazNucleo) + sizeof(int) + tamanioIdentificador;
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	memset(aux, '\0', tamanio);

	*((interfazNucleo*) aux) = ENTRADA_SALIDA;
	aux += sizeof(interfazNucleo);

	*((int*)aux) = operaciones;
	aux += sizeof(int);

	memcpy(aux, identificador, tamanioIdentificador);

	return tamanio;
}

void deserializarCargarPCB(void* parametrosSerializados, void* dataAdicional){
	int quantum = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);
	t_PCB* pcb = deserializarPCB(parametrosSerializados);

	cargarPCB(pcb, quantum);
}

void (*deserializadores[3])(void*, void*) = {deserializarCargarPCB, terminar, cancelar};

void procesarMensaje(void* mensaje, void* dataAdicional){
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux, dataAdicional);
	free(mensaje);
}
