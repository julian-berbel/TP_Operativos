/*
 * consola.c
 *
 *  Created on: 19/4/2016
 *      Author: utnso
 */

#include "cliente.h"
#include <commons/config.h>
#include <stdio.h>

int abrirConfiguracion();
char *puertoNucleo;
char *ipNucleo;

int main(){
	abrirConfiguracion();

	int socket_cliente = crear_socket_cliente("127.0.0.1", "5000");

	printf("Consola y Nucleo conectados\n");

	char* comando = malloc(10);

	do{
		scanf("%s", comando);
	}while(strcmp(comando, "prueba"));

	char * mensaje = malloc(100);
	strcpy(mensaje, "Hola!");

	enviar_string(socket_cliente, mensaje);

	close(socket_cliente);
	free(mensaje);
	free(comando);

	return 0;
}
int abrirConfiguracion(){
	t_config* configuracion= config_create("/home/utnso/tp-2016-1c-Hellfish-Group/consola/config/consola.config");
	ipNucleo = config_get_string_value(configuracion, "IP_NUCLEO");
	puertoNucleo= config_get_string_value(configuracion, "PUERTO_NUCLEO");
	config_destroy(configuracion);
	return 0;
}
