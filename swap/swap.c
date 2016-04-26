/*
 * swap.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */

#include <commons/config.h>
#include <stdio.h>
#include "servidor.h"

int abrirConfiguracion();
char *puertoSwap;
char *ipSwap;

int main(){
	abrirConfiguracion();

	int socket_servidor = crear_socket_servidor(ipSwap, puertoSwap);
	int socket_aceptado = recibirConexion(socket_servidor);

	printf("Swap y UMC conectados\n");

	char* mensaje;

	while(string_is_empty(mensaje = recibir_string_generico(socket_aceptado)));

	printf("Pase por la Swap - %s\n", mensaje);

	free(mensaje);
	close(socket_servidor);
	close(socket_aceptado);

	return 0;
}

int abrirConfiguracion(){
	t_config* configuracionSwap = config_create("/home/utnso/tp-2016-1c-Hellfish-Group/swap/config/swap.config");
	puertoSwap = config_get_string_value(configuracionSwap, "PUERTO_ESCUCHA");
	ipSwap = config_get_string_value(configuracionSwap, "IP_SWAP");
	return 0;
}
