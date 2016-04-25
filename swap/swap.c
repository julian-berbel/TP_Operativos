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
	struct sockaddr_storage their_addr;

	socklen_t addr_size;

	int socket_servidor = crear_socket_servidor(ipSwap, puertoSwap);

	int estado = listen(socket_servidor, 5);

	if(estado == -1){
		printf("Error al poner el servidor en listen\n");
		close(socket_servidor);
		return 1;
	}

	if(estado == 0){
		printf("Se puso el socket en listen\n");
	}

	addr_size = sizeof(their_addr);
	int socket_aceptado = accept(socket_servidor, (struct sockaddr *)&their_addr, &addr_size);

	if (socket_aceptado == -1){
		close(socket_servidor);
		printf("Error al aceptar conexion\n");
		return 1;
	}

	char *handshake = recibir_string_generico(socket_aceptado);
	printf("%s\n", handshake);

	handshake = recibir_string_generico(socket_aceptado);

	free(handshake);
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
