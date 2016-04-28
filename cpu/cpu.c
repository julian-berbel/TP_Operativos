/*
 * cpu.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */

#include "cliente.h"
#include <commons/config.h>
#include <stdio.h>

int abrirConfiguracion();
char* puertoNucleo;
char* ipNucleo;
char* puertoUMC;
char* ipUMC;

int main(){
	t_config* configuracionCPU = config_create("/home/utnso/tp-2016-1c-Hellfish-Group/cpu/config/cpu.config");
	abrirConfiguracion(configuracionCPU);

	int socket_nucleo = crear_socket_cliente(ipNucleo, puertoNucleo);

	printf("CPU y Nucleo conectados\n");

	int socket_umc = crear_socket_cliente(ipUMC, puertoUMC);

	printf("CPU y UMC conectados\n");

	char * mensaje;

	while(string_is_empty(mensaje = recibir_string_generico(socket_nucleo)));

	printf("Pase por la CPU - %s\n", mensaje);

	enviar_string(socket_umc, mensaje);

	close(socket_nucleo);
	free(mensaje);
	config_destroy(configuracionCPU);

	return 0;
}

int abrirConfiguracion(t_config* configuracionCPU){
	puertoNucleo = config_get_string_value(configuracionCPU, "PUERTO_NUCLEO");
	ipNucleo = config_get_string_value(configuracionCPU, "IP_NUCLEO");
	puertoUMC = config_get_string_value(configuracionCPU, "PUERTO_UMC");
	ipUMC = config_get_string_value(configuracionCPU, "IP_UMC");

	return 0;
}

