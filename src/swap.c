/*
 * swap.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */
#include <commons/config.h>
#include <stdio.h>

int main(){
	t_config* configuracion = config_create("/home/utnso/tp-2016-1c-Hellfish-Group/src/config/swap.config");
	int puerto= config_get_int_value(configuracion, "PUERTO_ESCUCHA");
	printf("%d", puerto);
	config_destroy(configuracion);
	return 0;
}
