/*
 * nucleo.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */

#include "servidor.h"
#include "cliente.h"
#include <commons/config.h>
#include <stdio.h>

int abrirConfiguracion();

char *puertoNucleo;
char *ipNucleo;
char *puertoCPU;

int main(){
	abrirConfiguracion();

	int socket_servidor = crear_socket_servidor(ipNucleo, puertoNucleo);
	int socket_consola = recibirConexion(socket_servidor);

	printf("Nucleo y Consola conectados\n");

	int otro_socket_servidor = crear_socket_servidor(ipNucleo, puertoCPU);
	int socket_cpu = recibirConexion(otro_socket_servidor);

	printf("Nucleo y CPU conectados\n");

	char* mensaje;

	while(string_is_empty(mensaje = recibir_string_generico(socket_consola)));

	printf("Pase por el Nucleo - %s\n", mensaje);

	enviar_string(socket_cpu, mensaje);

	free(mensaje);
	close(socket_consola);
	close(socket_servidor);
	close(socket_cpu);

	return 0;
}



int abrirConfiguracion(){
	t_config* configuracion= config_create("/home/utnso/tp-2016-1c-Hellfish-Group/nucleo/config/nucleo.config");
			puertoNucleo = config_get_string_value(configuracion, "PUERTO_PROG");
			ipNucleo = config_get_string_value(configuracion, "IP_NUCLEO");
			puertoCPU = config_get_string_value(configuracion, "PUERTO_CPU");
			return 0;
}
