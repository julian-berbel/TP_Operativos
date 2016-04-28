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
	t_config* configuracionConsola = config_create("/home/utnso/tp-2016-1c-Hellfish-Group/consola/config/consola.config");
	abrirConfiguracion(configuracionConsola);

	int socket_nucleo = crear_socket_cliente(ipNucleo, puertoNucleo);

	printf("Consola y Nucleo conectados\n");

	char* comando = string_new();

	do{
		gets(comando);
	}while(!string_starts_with(comando, "prueba "));

	char* mensaje = string_new();
	strcpy(mensaje, string_substring_from(comando ,7));

	enviar_string(socket_nucleo, mensaje);

	close(socket_nucleo);
	free(mensaje);
	free(comando);
	config_destroy(configuracionConsola);

	return 0;
}

int abrirConfiguracion(t_config* configuracionConsola){
	ipNucleo = config_get_string_value(configuracionConsola, "IP_NUCLEO");
	puertoNucleo = config_get_string_value(configuracionConsola, "PUERTO_NUCLEO");

	return 0;
}
