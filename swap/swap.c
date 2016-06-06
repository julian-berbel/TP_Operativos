#include <commons/config.h>
#include<commons/log.h>
#include"swap.h"
#include"binFile.h"

int main() {
	abrirConfiguracion();
	crearArchivoBinario(nombre_data, pagina_size, cant_paginas);

	log_info(logger, "Inicia proceso Swap");

	int socket_servidor = crear_socket_servidor(ipSwap, puertoSwap);
	int socket_umc = recibirConexion(socket_servidor);

	log_info(logger_pantalla, "Swap y UMC conectados");

	char* mensaje;

	while (string_is_empty(mensaje = recibir_string_generico(socket_umc)))
		;
	mensaje = "inicializar";

	if (strcmp(mensaje, "inicializar")) { //meter el programa en el archivo swap
		inicializar(1, 2, "asdf");
	} else if (strcmp(mensaje, "leer_pagina")) {
		//TODO completar
	} else if (strcmp(mensaje, "escribir_pagina")) {
		//TODO completar
	} else if (strcmp(mensaje, "finalizar_programa")) {
		//TODO completar
	}

	/*char * mensaje_logger = string_new();
	 string_append(&mensaje_logger, "Pase por la Swap - "); //->checkpoint 1
	 string_append(&mensaje_logger, mensaje);
	 log_info(logger_pantalla, mensaje_logger);
	 free(mensaje_logger);
	 printf("%s",mensaje);*/

	free(mensaje);
	close(socket_servidor);
	close(socket_umc);
	cerrar_todo();

	return 0;
}

void abrirConfiguracion() {
	configuracion_swap = config_create(RUTA_CONFIG);
	puertoSwap = config_get_string_value(configuracion_swap, "PUERTO_ESCUCHA");
	ipSwap = config_get_string_value(configuracion_swap, "IP_SWAP");
	nombre_data = config_get_string_value(configuracion_swap, "NOMBRE_SWAP");
	cant_paginas = config_get_int_value(configuracion_swap, "CANTIDAD_PAGINAS");
	pagina_size = config_get_int_value(configuracion_swap, "TAMAÑO_PAGINA");
	retardo_compactacion = config_get_int_value(configuracion_swap,
			"RETARDO_COMPACTACION");
	logger = log_create(RUTA_LOG, "Swap", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "Swap", true, LOG_LEVEL_INFO);

}

void cerrar_todo() {
	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracion_swap);
}

void inicializar(int id_programa, int paginas_requeridas, char* programa) {
	if (hayEspacio(paginas_requeridas)) {
		agregarProcesoALista(id_programa, paginas_requeridas);
		escribirArchivoBinario(programa);
	}
}
int hayEspacio(int paginas_requeridas) {
	if (cant_pags_disponibles() >= paginas_requeridas) {
		return 1;
	} else
		return 0;
}

void agregarProcesoALista(int id_programa, int paginas_requeridas) {
	int posicion,i,j;
	for(i = 0; i < paginas_requeridas; i++){
		for(j = 0; j < tamanioListaSwap(); j++){//recorre hasta el final de la lista
			if(lista_swap[i].pagina == NULL){ //fin de lista
				posicion = j;
				j = tamanioListaSwap();
			}
		}

		lista_swap[posicion].pid = id_programa;
		if (posicion > 0)
			lista_swap[posicion].pagina = lista_swap[posicion-1].pagina + 1;
		else
			lista_swap[posicion].pagina = 0;

		lista_swap[posicion].posicion = lista_swap[posicion].pagina * pagina_size;
		lista_swap[posicion].bit_uso = 1;
	}
}

int cant_pags_disponibles() {
	int paginas_usadas,i;

	for (i = 0; i < tamanioListaSwap(); i++) { //obtengo paginas usadas actualmente
		if(lista_swap[i].bit_uso == 1){
			paginas_usadas++;
		}
	}

	return tamanioListaSwap()	- paginas_usadas;
}

int tamanioListaSwap(){
	size_t t;
	t = sizeof(lista_swap) / sizeof(t_swap);
	return (int)t;
}
