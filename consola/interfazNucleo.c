/*
 * interfazPropia.c
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#include "interfazNucleo.h"

void deserializarImprimir(void* parametrosSerializados, void* dataAdicional){
	char* mensaje = parametrosSerializados;

	log_info(logger, "Deserializado: Imprimir: mensaje: %s", mensaje);
	imprimir(mensaje);
}

void (*deserializadores[2])(void*, void*) = {deserializarImprimir, terminar};

void procesarMensaje(void* mensaje, void* dataAdicional){
	log_info(logger, "Procesando mensaje");
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);
	(*deserializadores[tipo])(aux, dataAdicional);
	free(mensaje);
}
