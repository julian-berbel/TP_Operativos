/*
 * socketComms.c
 *
 *  Created on: 25/4/2016
 *      Author: utnso
 */

#include "socketComms.h"

char* recibir_string_generico(int socket_aceptado){
	return (char*) recibir(socket_aceptado);
}

void enviar_string(int socket, char* mensaje){
	int tamanio = string_length(mensaje) + 1;

	enviar(socket, (void*) mensaje, tamanio);
}

void enviar(int socket, void* cosaAEnviar, int tamanio){
	void* mensaje = malloc(sizeof(int) + tamanio);
	void* aux = mensaje;
	*((int*)aux) = tamanio;
	aux += sizeof(int);
	memcpy(aux, cosaAEnviar, tamanio);

	send(socket, mensaje, sizeof(int) + tamanio, 0);
	free(mensaje);
}

void* recibir(int socket){
	void* recibido = malloc(sizeof(int));

	memset(recibido, 0, sizeof(int));

	read(socket, recibido, sizeof(int));

	int tamanioDelMensaje = *((int*)recibido);

	free(recibido);

	recibido = malloc(tamanioDelMensaje);

	int bytesRecibidos = 0;

	while(bytesRecibidos < tamanioDelMensaje) bytesRecibidos += read(socket, (recibido + bytesRecibidos), (tamanioDelMensaje - bytesRecibidos));

	return recibido;
}
