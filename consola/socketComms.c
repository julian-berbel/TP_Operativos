/*
 * socketComms.c
 *
 *  Created on: 25/4/2016
 *      Author: utnso
 */

#include "socketComms.h"

char* recibir_string_generico(int socket_aceptado){
	char* mensaje = string_new();
	char* acumulador = malloc(100);
	while(recv(socket_aceptado, acumulador, 99, 0) > 0) string_append(&mensaje, acumulador);

	free(acumulador);
	return mensaje;
}

int enviar_string(int socket, char * mensaje){
	int e = string_length(mensaje);
	int check_error = send(socket, mensaje, e, 0);

	if(check_error < 0){
	return 1;
	}
    return 0;
}
