/*
 * umc.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */
#include <commons/config.h>
#include <stdio.h>
int abrirConfiguracion();
int puertoUmc;
int main(){
	abrirConfiguracion();
	return 0;
}
	int abrirConfiguracion(){
		t_config* configuracion= config_create("/home/utnso/tp-2016-1c-Hellfish-Group/umc/config/umc.config");
				puertoUmc = config_get_int_value(configuracion, "PUERTO");
		return 0;
	}


