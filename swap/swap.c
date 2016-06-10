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

	//inicializo las lista de utilizados en vacio porque no hay nada todavia
	//inicializo a la de disponible con el tamaño total de la swap
	espacioUtilizado = list_create();
	//espacioDisponible = list_create();
	t_swap* swap = malloc(sizeof(t_swap));
	swap->bit_uso = 0;
	swap->offset = 0;
	swap->pagina = 0;
	swap->pid = 0;
	swap->posicion = 0;
	list_add(espacioUtilizado, (void*) swap);

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
	if (cant_pags_disponibles() >= paginas_requeridas) { //antes estaba hay espacio
		if(hayQueCompactar(paginas_requeridas)){
			compactar();
		}
		agregarProcesoALista(id_programa, paginas_requeridas);
		escribirArchivoBinario(programa);
	}else{
		//decirle a la umc que no se pudo iniciar porque no hay espacio
	}
}

/*int hayEspacio(int paginas_requeridas) {
	if (cant_pags_disponibles() >= paginas_requeridas) {
		return 1;
	} else
		return 0;
}*/

int hayQueCompactar(int paginas_requeridas){
	t_list espaciosLibres = list_filter(espacioUtilizado,estaUtilizado(espacioUtilizado));
	int cantidadDeEspaciosLibres = list_size(espaciosLibres);
	if(paginas_requeridas <= cantidadDeEspaciosLibres){
		return 1;
	}
	else { return 0;
	}
}

int estaUtilizado(t_list* espacioUtilizado){
	int cantidadDeProcesos = list_size(espacioUtilizado);
	bool esta = 1;
	int i = 0;
	for(i=0; i < cantidadDeProcesos; i++){
		t_swap* datos = list_get(espacioUtilizado, i);
		if(datos->bit_uso == 0){
			esta = 0;
		}
	} return esta;
}

void compactar(){
	//ver si el bit de uso del primer elemento esta libre (1), si está guardo esa pagina/marco
	//cuando recorri toda la lista, al marco libre lo ocupo por el mas proximo ocupado y al ocupado
	//lo lleno de ceros...

	sleep(retardo_compactacion);
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
