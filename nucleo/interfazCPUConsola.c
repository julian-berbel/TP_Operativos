/*
 * interfazCPU.c
 *
 *  Created on: 7/6/2016
 *      Author: utnso
 */

#include "interfazCPUConsola.h"

int serializarCargarPCB(t_PCB* pcb, void** serializacion){
	log_info(logger, "Serializando: Cargar PCB, pid: %d", pcb->pid);
	int tamanio = sizeof(interfazCPU) + calcularTamanioPCB(pcb) + sizeof(int);
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	*((interfazCPU*) aux) = CARGAR_PCB;
	aux += sizeof(interfazCPU);

	*((int*) aux) = quantum;
	aux += sizeof(int);

	serializarPCB(pcb, &aux);

	return tamanio;
}

int serializarTerminar(void** serializacion){
	log_info(logger, "Serializando: Terminar");
	*serializacion = malloc(sizeof(interfazCPU));
	*(interfazCPU*)*serializacion = 1;

	return sizeof(interfazCPU);
}

int serializarImprimir(char* mensaje, void** serializacion){
	log_info(logger, "Serializando: Imprimir: mensaje: %s", mensaje);
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
	log_info(logger, "Serializando: Continuar ejecucion");
	int tamanio = sizeof(interfazCPU) + sizeof(int);
	*serializacion = malloc(tamanio);
	void* aux = *serializacion;

	*((interfazCPU*) aux) = CONTINUAR_EJECUCION;
	aux += sizeof(interfazCPU);

	*((int*) aux) = quantum;

	return tamanio;
}

int serializarDesalojar(void** serializacion){
	log_info(logger, "Serializando: Desalojar");
	int tamanio = sizeof(int);
	*serializacion = malloc(tamanio);
	/*memset(*serializacion, 0, tamanio);
	int i = DESALOJAR;
	memcpy(*serializacion, &i, tamanio);*/

	//*((interfazCPU*) serializacion) = DESALOJAR;

	*(interfazCPU*)*serializacion = DESALOJAR;

	return tamanio;
}

void deserializarImprimir(void* parametrosSerializados, void* cpu){
	char* mensaje = parametrosSerializados;

	log_info(logger, "Deserializado: Imprimir: mensaje: %s", mensaje);
	imprimir(mensaje, cpu);
}

void deserializarQuantumTerminado(void* parametrosSerializados, void* cpu){
	log_info(logger, "Deserializado: Quantum Terminado");
	quantumTerminado(cpu);
}

void deserializarObtenerValor(void* parametrosSerializados, void* cpu){
	char* identificador = parametrosSerializados;

	log_info(logger, "Deserializado: Obtener Valor: id: %s", identificador);
	obtener_valor(identificador, cpu);
}

void deserializarGrabarValor(void* parametrosSerializados, void* cpu){
	int valorAGrabar = *((int*)parametrosSerializados);
	parametrosSerializados += sizeof(int);
	char* identificador = parametrosSerializados;

	log_info(logger, "Deserializado: Grabar Valor: id: %s, valor: %d", identificador, valorAGrabar);
	grabar_valor(identificador, valorAGrabar);
}

void deserializarWait(void* parametrosSerializados, void* cpu){
	char* identificador = parametrosSerializados;

	log_info(logger, "Deserializado: Wait: id: %s", identificador);
	esperar(identificador, cpu);
}

void deserializarSignal(void* parametrosSerializados, void* cpu){
	char* identificador = parametrosSerializados;

	log_info(logger, "Deserializado: Signal: id: %s", identificador);
	avisar(identificador);
}

void deserializarEntradaSalida(void* parametrosSerializados, void* cpu){
	int operaciones = *((int*)parametrosSerializados);
	parametrosSerializados += sizeof(int);
	char* identificador = parametrosSerializados;

	log_info(logger, "Deserializado: Entrada/Salida: id: %s, operaciones: %d", identificador, operaciones);
	entradaSalida(identificador, operaciones, cpu);
}

void deserializarCerrarCPU(void* parametrosSerializados, void* cpu){
	log_info(logger, "Deserializado: Cerrar CPU");
	cerrarCPU(cpu, parametrosSerializados);
}

void deserializarProgramaTerminado(void* parametrosSerializados, void* cpu){
	log_info(logger, "Deserializado: Programa Terminado");
	programaTerminado(cpu);
}

void (*deserializadores[9])(void*, void*) = {	deserializarImprimir, deserializarQuantumTerminado, deserializarObtenerValor, deserializarGrabarValor,
												deserializarWait, deserializarSignal, deserializarEntradaSalida, deserializarCerrarCPU, deserializarProgramaTerminado};

void procesarMensaje(void* mensaje, void* dataAdicional){
	log_info(logger, "Procesando mensaje");
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux, dataAdicional);
	free(mensaje);
}
