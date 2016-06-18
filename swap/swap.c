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
	int iterator;

	for (iterator = 0; iterator < cant_paginas; iterator++) {
		t_swap* swap = malloc(sizeof(t_swap));
		swap->bit_uso = 0;
		//swap->offset = 0;
		swap->pagina = iterator;
		//swap pos = 0;
		list_add(espacioTotal, (void*) swap);
		free(swap); //liberar swap
	}

	listaDeProcesos = list_create();

	if (strcmp(mensaje, "inicializar") == 0) { //meter el programa en el archivo swap
		inicializar(1, 1, "asdf");
	} else if (strcmp(mensaje, "leer_pagina") == 0) {
		//TODO completar
	} else if (strcmp(mensaje, "escribir_pagina") == 0) {
		//TODO completar
	} else if (strcmp(mensaje, "finalizar_programa") == 0) {
		//TODO completar
	}

	/*while(!flagTerminar){
	 mensaje = recibir(socket_umc);      // se usa asi
	 procesarMensaje(mensaje, NULL);
	 }*/
	//enviar(socket_umc, mensaje, tamanio);   ---> devolver pagina a la umc
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
	if (cant_pags_disponibles() >= paginas_requeridas) {
		if (hayQueCompactar(paginas_requeridas)) {
			compactar();
		}
		agregarProcesoALista(id_programa, paginas_requeridas);
		escribirArchivoBinario(programa);
	} else {
		log_info(logger, "No hay espacio en la swap");
		// TODO decirle a la umc que no se pudo iniciar porque no hay espacio
	}
}

/*int hayEspacio(int paginas_requeridas) {
 if (cant_pags_disponibles() >= paginas_requeridas) {
 return 1;
 } else
 return 0;
 }*/

Bool noEstaUtilizado(t_swap* swap) {

	if (swap->bit_uso == 0) {
		return true;
	} else {
		return false;
	}
}

t_list* espaciosLibres() {
	t_list* espaciosLibres = list_filter(espacioTotal, (void*) noEstaUtilizado);
	return espaciosLibres;
}

int hayQueCompactar(int paginas_requeridas) {
	//filtrar por no estautilizado entonces despues tenemos que ver si estan los numeros
	//de las paginas en orden o continuos y despues tenemos que comparar con paginas requeridas
	//para ver si hay una cantidad minima de paginas requeridas juntas

	return !(estanPaginasContinuas(espaciosLibres(), paginas_requeridas));

}

int estanPaginasContinuas(t_list* espaciosLibres, int paginas_requeridas) {
	int i;
	int contadorDeEspaciosContiguos = 1;
	int cantidadEspaciosLibres = list_size(espaciosLibres);
	for (i = 0; i < cantidadEspaciosLibres - 1; i++) {
		t_swap* swap = list_get(espaciosLibres, i);
		t_swap* swapPosterior = list_get(espaciosLibres, i + 1);
		if (swap->pagina == swapPosterior->pagina - 1) {
			contadorDeEspaciosContiguos++;
		} else {
			contadorDeEspaciosContiguos = 1;
		}
	}
	if (contadorDeEspaciosContiguos >= paginas_requeridas) {
		return 0;
	} else {
		return 1;
	}
}

t_list* sacarRepetidos(t_list* lista) {
	int i, j, tamanioLista;
	tamanioLista = list_size(lista);
	for (i = 0; i < tamanioLista; i++) {
		t_swap* primerElemento = list_get(lista, i);
		for (j = i + 1; j <= tamanioLista - 1; j++) {
			t_swap* siguienteElemento = list_get(lista, j);
			if (primerElemento->pagina == siguienteElemento->pagina) {
				list_remove(lista, j + 1);
				j=j-1;
			}
		}

	}
	return lista;
}

t_list* paginasAReemplazar(t_list* espaciosLibres, int paginas_requeridas) {
	int i;
	t_list* paginasContiguas = list_create();
	int cantidadEspaciosLibres = list_size(espaciosLibres);
	for (i = 0; i < cantidadEspaciosLibres - 1 && i < paginas_requeridas; i++) {
		t_swap* swap = list_get(espaciosLibres, i);
		t_swap* swapPosterior = list_get(espaciosLibres, i + 1);
		if (swap->pagina == swapPosterior->pagina - 1) {
			list_add(paginasContiguas, swap);
			list_add(paginasContiguas, swapPosterior);
		}


	}
	t_list* pagsContSinRepetidos = sacarRepetidos(paginasContiguas);
	return pagsContSinRepetidos;

}

//AHORA NO SE ESTA UTILIZANDO EN NINGUN LADO
Bool estaUtilizado() {
	int cantidadDeAlgo = list_size(espacioTotal);
	Bool esta = true;
	int i = 0;
	for (i = 0; i < cantidadDeAlgo; i++) {
		t_swap* algo = list_get(espacioTotal, i);
		if (algo->bit_uso == 0) {
			esta = false;
		}
	}
	return esta;
}

void compactar() {
	// TODO
	//ver si el bit de uso del primer elemento esta libre (1), si está guardo esa pagina/marco
	//cuando recorri toda la lista, al marco libre lo ocupo por el mas proximo ocupado y al ocupado
	//lo lleno de ceros...

	sleep(retardo_compactacion / 1000);
}

void agregarProcesoALista(int id_programa, int paginas_requeridas) {
	int j;
	t_list* listaLibres = espaciosLibres();
	if (paginas_requeridas == 1) {
		t_swap* primerEspacioLibre = list_get(listaLibres, 0);
		primerEspacioLibre->bit_uso = 1;
		list_replace(espacioTotal, primerEspacioLibre->pagina,
				primerEspacioLibre);
	} else {
		t_list* pagsAReemplazar = paginasAReemplazar(listaLibres,paginas_requeridas);
		for (j = 0 ; j < paginas_requeridas; j++) {
			t_swap* swap = list_get(pagsAReemplazar, j);
			swap->bit_uso = 1;
			list_replace(espacioTotal, swap->pagina, swap);

			t_proceso* proceso = malloc(sizeof(t_proceso));
			proceso->pagina = swap->pagina ;
			proceso->pid = id_programa;
			list_add(listaDeProcesos, (void*) proceso);
			free(proceso); //liberar proceso
		}

	}
}

void finalizar(int id_programa) {

	int j;
	for (j = 0; j < list_size(listaDeProcesos); j++) {
		t_proceso* proceso = list_get(listaDeProcesos, j);
		if (proceso->pid == id_programa) {
			int paginaALiberar = proceso->pagina;
			list_remove(listaDeProcesos, j);
			t_swap* swap = list_get(espacioTotal, paginaALiberar);
			swap->bit_uso = 0;
			list_replace(espacioTotal, paginaALiberar, swap);
		}

	}

}

int cant_pags_disponibles() {
	int cantidadDisponibles = list_size(espaciosLibres());
	return cantidadDisponibles;
}


void leer_pagina(int id_programa, int num_pagina) {

}

void escribir_pagina(int id_programa, int num_pagina, char* buffer) {
// escribir archivo binario
}

void terminar() {

}
