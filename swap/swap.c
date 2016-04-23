/*
 * swap.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */
#include <commons/config.h>
#include <stdio.h>
int abrirConfiguracion();
int puertoSwap;

int main(){
	abrirConfiguracion();
	return 0;
}
int abrirConfiguracion(){
	t_config* configuracionSwap = config_create("/home/utnso/tp-2016-1c-Hellfish-Group/swap/config/swap.config");
		puertoSwap= config_get_int_value(configuracionSwap, "PUERTO_ESCUCHA");
		return 0;
}
