/*
 * interfazPropia.c
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#include "interfazNucleo.h"

void deserializarImprimir(void* parametrosSerializados){
	char* mensaje = parametrosSerializados;

	imprimir(mensaje);
}

void (*deserializadores[2])(void*) = {terminar, deserializarImprimir};

void procesarMensaje(void* mensaje){
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux);
	free(mensaje);
}

int serializarCancelar(void** serializacion) {
	*serializacion = malloc(sizeof(interfazPropia)) + sizeof(int);
	void* aux = *serializacion;

	*(interfazPropia*)aux = CANCELAR;
	aux += sizeof(interfazPropia);

	*(int*)aux = pid;

	return sizeof(interfazPropia);
}
