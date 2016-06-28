/*
 * interfazCPU.c
 *
 *  Created on: 7/6/2016
 *      Author: utnso
 */

#include "interfazNucleo.h"

int serializarImprimir(char* mensaje, void** serializacion){
	log_info(logger, "Serializando: Imprimir: mensaje: %s", mensaje);
	int tamanioMensaje = sizeof(char) * (string_length(mensaje) + 1);
	int tamanio = sizeof(interfazNucleo) + tamanioMensaje;
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	memset(aux, '\0', tamanio);

	*((interfazNucleo*) aux) = IMPRIMIR;
	aux += sizeof(interfazNucleo);

	memcpy(aux, mensaje, tamanioMensaje);

	return tamanio;
}

int serializarQuantumTerminado(void** serializacion){
	log_info(logger, "Serializando: Quantum Terminado");
	int tamanio = sizeof(interfazNucleo);
	*serializacion = malloc(tamanio);

	*(interfazNucleo*)*serializacion = QUANTUM_TERMINADO;

	return tamanio;
}

int serializarObtenerValor(char* identificador, void** serializacion){
	log_info(logger, "Serializando: Obtener Valor: id: %s", identificador);
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
	log_info(logger, "Serializando: Grabar Valor: id: %s, valor: %d", identificador, valor);
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
	log_info(logger, "Serializando: Wait: id: %s", identificador);
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
	log_info(logger, "Serializando: Signal: id: %s", identificador);
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
	log_info(logger, "Serializando: Entrada/Salida: id: %s, operaciones: %d", identificador, operaciones);
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

int serializarCerrarCPU(void** serializacion){
	log_info(logger, "Serializando: Cerrar CPU");
	int tamanio = sizeof(interfazNucleo);
	*serializacion = malloc(tamanio);

	*(interfazNucleo*)*serializacion = CERRAR_CPU;

	return tamanio;
}

int serializarProgramaTerminado(void** serializacion){
	log_info(logger, "Serializando: Programa Terminado");
	int tamanio = sizeof(interfazNucleo);
	*serializacion = malloc(tamanio);

	*(interfazNucleo*)*serializacion = PROGRAMA_TERMINADO;

	return tamanio;
}

void deserializarCargarPCB(void* parametrosSerializados, void* dataAdicional){
	int quantum = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);
	t_PCB* pcb = deserializarPCB(parametrosSerializados);

	log_info(logger, "Deserializado: Cargar PCB: pid: %d", pcb->pid);
	cargarPCB(pcb, quantum);
}

void deserializarContinuarEjecucion(void* parametrosSerializados, void* dataAdicional){
	int quantum = *((int*) parametrosSerializados);

	log_info(logger, "Deserializado: Continuar Ejecucion: quantum: %d", quantum);
	continuarEjecucion(quantum);
}

void (*deserializadores[4])(void*, void*) = {deserializarCargarPCB, terminar, deserializarContinuarEjecucion, desalojar};

void procesarMensaje(void* mensaje, void* dataAdicional){
	log_info(logger, "Procesando Mensaje");
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux, dataAdicional);
	free(mensaje);
}
