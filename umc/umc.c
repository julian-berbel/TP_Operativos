/*
 * umc.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */
#include <commons/config.h>
#include <stdio.h>
#include "servidor.h"
#include "cliente.h"

int abrirConfiguracion();
char* puertoUmc;
char* ipUmc;
char* ipSwap;
char* puertoSwap;

int main(){
	t_config* configuracionUMC = config_create("/home/utnso/tp-2016-1c-Hellfish-Group/umc/config/umc.config");
	abrirConfiguracion(configuracionUMC);

	int socket_swap = crear_socket_cliente(ipSwap, puertoSwap);

	printf("UMC y Swap conectados\n");

	int socket_servidor = crear_socket_servidor(ipUmc, puertoUmc);
	int socket_cpu = recibirConexion(socket_servidor);

	printf("UMC y CPU conectados\n");

	char* mensaje;

	while(string_is_empty(mensaje = recibir_string_generico(socket_cpu)));

	printf("Pase por la UMC - %s\n", mensaje);

	enviar_string(socket_swap, mensaje);

	free(mensaje);
	close(socket_swap);
	close(socket_servidor);
	close(socket_cpu);
	config_destroy(configuracionUMC);

	return 0;
}

int abrirConfiguracion(t_config* configuracionUMC){
	puertoUmc = config_get_string_value(configuracionUMC, "PUERTO");
	ipUmc = config_get_string_value(configuracionUMC, "IP_UMC");
	puertoSwap = config_get_string_value(configuracionUMC, "PUERTO_SWAP");
	ipSwap = config_get_string_value(configuracionUMC, "IP_SWAP");

	return 0;
}


