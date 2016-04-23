/*
 * swap.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */
#include <commons/config.h>
#include <stdio.h>

int main(){
	t_config* configuracionSwap = config_create("/home/utnso/tp-2016-1c-Hellfish-Group/src/config/swap.config");
	int puertoSwap= config_get_int_value(configuracionSwap, "PUERTO_ESCUCHA");

	t_config* configuracionNucleo= config_create("/home/utnso/tp-2016-1c-Hellfish-Group/src/config/nucleo.config");
		int puertoNucleo = config_get_int_value(configuracionNucleo, "PUERTO_PROG");
		printf("%d",puertoNucleo);

	t_config* configuracionConsola= config_create("/home/utnso/tp-2016-1c-Hellfish-Group/src/config/consola.config");
		int puertoConsola = config_get_int_value(configuracionConsola, "PUERTO");
		printf("%d",puertoConsola);
	printf("puerto de SWAP: %d", puertoSwap);
	//config_destroy(configuracionSwap);
	return 0;
}
