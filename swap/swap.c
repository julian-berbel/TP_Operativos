#include <commons/config.h>
#include<commons/log.h>
#include"swap.h"
#include"binFile.h"

int main(){
	abrirConfiguracion();
	crearArchivoBinario(nombre_data,pagina_size,cant_paginas);
	log_info(logger, "Inicia proceso Swap");

	int socket_servidor = crear_socket_servidor(ipSwap, puertoSwap);
	int socket_umc = recibirConexion(socket_servidor);

	log_info(logger_pantalla, "Swap y UMC conectados");

	char* mensaje;

	while(string_is_empty(mensaje = recibir_string_generico(socket_umc)));

	char * mensaje_logger = string_new();
	string_append(&mensaje_logger, "Pase por la Swap - ");
	string_append(&mensaje_logger, mensaje);
	log_info(logger_pantalla, mensaje_logger);
	free(mensaje_logger);

	free(mensaje);
	close(socket_servidor);
	close(socket_umc);
	cerrar_todo();

	return 0;
}

void abrirConfiguracion(){
	configuracion_swap = config_create(RUTA_CONFIG);
	puertoSwap = config_get_string_value(configuracion_swap, "PUERTO_ESCUCHA");
	ipSwap = config_get_string_value(configuracion_swap, "IP_SWAP");
	nombre_data = config_get_string_value(configuracion_swap, "NOMBRE_SWAP");
	cant_paginas = config_get_int_value(configuracion_swap, "CANTIDAD_PAGINAS");
	pagina_size = config_get_int_value(configuracion_swap, "TAMAÑO_PAGINA");
	retardo_compactacion = config_get_int_value(configuracion_swap, "RETARDO_COMPACTACION");
	logger = log_create(RUTA_LOG, "Swap", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "Swap", true, LOG_LEVEL_INFO);

	printf("%s\n", puertoSwap);
	printf("%s\n", ipSwap);
	printf("%s\n", nombre_data);
	printf("%d\n", cant_paginas);
	printf("%d\n", pagina_size);
	printf("%d\n", retardo_compactacion);


}

void cerrar_todo(){
	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracion_swap);
}



