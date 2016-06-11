#include"swap.h"


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

	//inicializo las lista de utilizados en vacio porque no hay nada todavia? o como esta ahora???
	espacioTotal = list_create();
	t_swap* swap = malloc(sizeof(t_swap));
	swap->bit_uso = 0;
	//swap->offset = 0;
	swap->pagina = 0;
	swap->pid = 0;
	//swap pos = 0;
	list_add(espacioTotal, (void*) swap);

	if (strcmp(mensaje, "inicializar")) { //meter el programa en el archivo swap
		inicializar(1, 1, "asdf");
	} else if (strcmp(mensaje, "leer_pagina")) {
		//TODO completar
	} else if (strcmp(mensaje, "escribir_pagina")) {
		//TODO completar
	} else if (strcmp(mensaje, "finalizar_programa")) {
		//TODO completar
	}

	/*while(!flagTerminar){
		mensaje = recibir(socket_umc);      // se usa asi
		procesarMensaje(mensaje, NULL);
	}*/


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
		// TODO decirle a la umc que no se pudo iniciar porque no hay espacio
	}
}

/*int hayEspacio(int paginas_requeridas) {
	if (cant_pags_disponibles() >= paginas_requeridas) {
		return 1;
	} else
		return 0;
}*/

int hayQueCompactar(int paginas_requeridas){
	t_list* espaciosLibres = list_filter(espacioTotal,(void*) estaUtilizado);
	int cantidadDeEspaciosLibres = list_size(espaciosLibres);
	if(paginas_requeridas <= cantidadDeEspaciosLibres){
		return 1;
	}
	else { return 0;
	}
}

//NO ES ALGO, BUSCAR MEJOR ABSTRACCION
Bool estaUtilizado(){
	int cantidadDeAlgo = list_size(espacioTotal);
	Bool esta = true;
	int i = 0;
	for(i=0; i < cantidadDeAlgo; i++){
		t_swap* algo = list_get(espacioTotal, i);
		if(algo->bit_uso == 0){
			esta = false;
		}
	} return esta;
}

void compactar(){
	// TODO
	//ver si el bit de uso del primer elemento esta libre (1), si está guardo esa pagina/marco
	//cuando recorri toda la lista, al marco libre lo ocupo por el mas proximo ocupado y al ocupado
	//lo lleno de ceros...

	sleep(retardo_compactacion/1000);
}




void agregarProcesoALista(int id_programa, int paginas_requeridas) {

// tiene que recorrer como en estautilizado y reemplazar? o directamente agregarlo al final?
	//como le agrego la pagina? como se la ultima posicion de la lista?

	int cantidadAlgo = list_size(espacioTotal);
	int i;
	for(i=0; i < paginas_requeridas; i++){
			t_swap* proceso = malloc(sizeof(proceso));
			proceso->pid = id_programa;
			proceso->bit_uso = 1;
			proceso->pagina = (cantidadAlgo + 1);
			paginas_requeridas --;
			list_add(espacioTotal, proceso);



		}
	}



void finalizar(int id_programa){
	int cantidadAlgo = list_size(espacioTotal);
		int i;
		for(i=0; i < cantidadAlgo; i++){
			t_swap* proceso = list_get(espacioTotal, i);
			if(proceso->pid == id_programa){
				proceso->bit_uso = 0;
				proceso->pid = NULL;
				//TODO borrarArchivoBinario(proceso->pagina);
			}

		}


}

//NO ES ALGO, BUSCAR MEJOR ABSTRACCION
Bool noEstaUtilizado(){
	int cantidadDeAlgo = list_size(espacioTotal);
	Bool esta = true;
	int i = 0;
	for(i=0; i < cantidadDeAlgo; i++){
		t_swap* algo = list_get(espacioTotal, i);
		if(algo->bit_uso == 1){
			esta = false;
		}
	} return esta;
}

int cant_pags_disponibles() {
	t_list* espaciosOcupados = list_filter(espacioTotal,(void*) noEstaUtilizado);
	int paginas_usadas = sizeof(espaciosOcupados);
	return cant_paginas - paginas_usadas;

}

/*int tamanioListaSwap(){
	size_t t;
	t = sizeof(lista_swap) / sizeof(t_swap);
	return (int)t;
}*/

void leer_pagina(int id_programa, int num_pagina){

}

void escribir_pagina(int id_programa, int num_pagina, char* buffer){

}

void terminar(){

}
