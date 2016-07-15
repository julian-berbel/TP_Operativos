#include "swap.h"

int main() {
	abrirConfiguracion();
	crearArchivoBinario(nombre_data, pagina_size, cant_paginas);
	abrirArchivoBinario();

	log_info(logger, "Inicia proceso Swap");

	int socket_servidor = crear_socket_servidor(ipSwap, puertoSwap);
	socket_umc = recibirConexion(socket_servidor);

	log_info(logger, "Swap y UMC conectados");

	//inicializo las lista de espacioTotal con la cantidad de paginas y el bit de uso en 0
	//creo la lista de procesos para su posterior uso
	espacioTotal = list_create();
	int iterator;

	for (iterator = 0; iterator < cant_paginas; iterator++) {
		t_swap* swap = malloc(sizeof(t_swap));
		swap->bit_uso = 0;
		swap->pagina = iterator;
		list_add(espacioTotal, (void*) swap);
	}

	listaDeProcesos = list_create();

	void* mensaje;

	while (!flagTerminar) {
		mensaje = recibir(socket_umc);
		if (!mensaje)
			break;
		procesarMensaje(mensaje, NULL);
	}

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
	retardo_acceso = config_get_int_value(configuracion_swap, "RETARDO_ACCESO");
	logger = log_create(RUTA_LOG, "Swap", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "Swap", true, LOG_LEVEL_INFO);

}

void cerrar_todo() {
	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracion_swap);
	fclose(archivo);
	int i;
	for(i = (list_size(espacioTotal) - 1); i >= 0; i--){
		t_swap* swap = list_remove(espacioTotal, i);
		free(swap);
	}
	for(i = (list_size(listaDeProcesos) - 1); i >= 0; i--){
		t_proceso* proceso = list_remove(listaDeProcesos, i);
		free(proceso);
	}
	list_destroy(espacioTotal);
	list_destroy(listaDeProcesos);
}

void inicializar(int id_programa, int paginas_requeridas, char* programa) { //testeado
	log_info(logger, "Inicializando proceso %d", id_programa);
	if (cant_pags_disponibles() >= paginas_requeridas) {
		if (hayQueCompactar(paginas_requeridas)) {
			compactar();
		}


		//llenamos de espacios hasta que ocupe toda la pagina
		//char* caracteresDeRelleno = string_new();
		int cantidadDeEspacios;
		cantidadDeEspacios = (paginas_requeridas * pagina_size
				- string_length(programa));
		char* caracteresDeRelleno = string_repeat(' ', cantidadDeEspacios);
		char* programaRelleno = string_new();
		string_append(&programaRelleno, programa);
		string_append(&programaRelleno, caracteresDeRelleno);
		//string_append(&programaRelleno, "\0");
		free(caracteresDeRelleno);

		agregarProcesoALista(id_programa, paginas_requeridas, programaRelleno);
		free(programaRelleno);

		enviar_string(socket_umc, "OK");
		log_info(logger, "Proceso inicializado %d", id_programa);
	} else {
		log_info(logger, "No hay espacio en la swap");
		enviar_string(socket_umc, "NO OK");
	}
}

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
	t_list* espacioLibres = espaciosLibres();
	int i = estanPaginasContinuas(espacioLibres, paginas_requeridas);
	int j;
	for(j = (list_size(espacioLibres) - 1); j >= 0; j--){
		list_remove(espacioLibres, j);
	}
	list_destroy(espacioLibres);
	return !i;

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
		return 1;
	} else {
		return 0;
	}
}

t_list* sacarRepetidos(t_list* lista, int paginas_requeridas) {
	t_list* sinRepetidos = list_create();
	t_swap* primerElemento = list_get(lista, 0);
	int pagina;
	pagina = primerElemento->pagina;
	int i;
	for (i = 0; i < paginas_requeridas; i++) {
		t_swap* swap = malloc(sizeof(t_swap));
		swap->bit_uso = 0;
		swap->pagina = pagina + i;
		list_add(sinRepetidos, swap);
	}
	return sinRepetidos;
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
	t_list* pagsContSinRepetidos = sacarRepetidos(paginasContiguas,
			paginas_requeridas);
	for(i = (list_size(paginasContiguas) - 1); i >= 0; i--){
		list_remove(paginasContiguas, i);
	}
	list_destroy(paginasContiguas);
	return pagsContSinRepetidos;

}

void compactar() {
	log_info(logger, "Iniciando compactacion");
	int cantidadEspaciosLibres = list_size(espaciosLibres());
	while (!(estanPaginasContinuas(espaciosLibres(), cantidadEspaciosLibres))) {
		recorrerYModificarArchivoYListas();
	}

	usleep(retardo_compactacion * 1000);
	log_info(logger, "Compactacion finalizada");
}

void recorrerYModificarArchivoYListas() {

	char* paginaAMoverEnUso/*, paginaAMoverSinUso*/;
	int i;
	for (i = 0; i < list_size(espacioTotal) - 1; i++) { //Recorre la lista de espacioTotal para correr los espacios libres al final del archivo y de la lista
		t_swap* swap = list_get(espacioTotal, i);
		t_swap* swapSig = list_get(espacioTotal, i + 1);
		if (swap->bit_uso == 0 && swapSig->bit_uso == 1) {
			int j;
			for (j = 0; j < list_size(listaDeProcesos); j++) { //Recorre lista de procesos para asignarle la nueva pagina
				t_proceso* proceso = list_get(listaDeProcesos, j);
				if (proceso->pagina == swapSig->pagina) {
					proceso->pagina = swap->pagina;
					list_replace(listaDeProcesos, j, proceso);
					j = list_size(listaDeProcesos); //para que salga del for y no recorra toda la lista
				}
			}
			paginaAMoverEnUso = leerArchivoBinarioEnPagina(swapSig->pagina);
			escribirArchivoBinarioEnPag(swap->pagina, paginaAMoverEnUso);
			swap->bit_uso = 1;
			swapSig->bit_uso = 0;
			t_swap* swapReemplazado = list_replace(espacioTotal, i, swap);
			free(swapReemplazado);
			swapReemplazado = list_replace(espacioTotal, i + 1, swapSig);
			free(swapReemplazado);

		}
	}
}

void agregarProcesoALista(int id_programa, int paginas_requeridas, char* programa) {
	int j;
	t_list* listaLibres = espaciosLibres();
	if (paginas_requeridas == 1) {
		t_swap* primerEspacioLibre = list_get(listaLibres, 0);
		primerEspacioLibre->bit_uso = 1;
		t_swap* swapReemplazado = list_replace(espacioTotal, primerEspacioLibre->pagina,
				primerEspacioLibre);
		free(swapReemplazado);

		t_proceso* proceso = malloc(sizeof(t_proceso));
		proceso->pagina = primerEspacioLibre->pagina;
		proceso->pid = id_programa;

		escribirArchivoBinarioEnPag(proceso->pagina, programa);

		list_add(listaDeProcesos, (void*) proceso);
		//free(proceso); //liberar proceso

	} else {
		t_list* pagsAReemplazar = paginasAReemplazar(listaLibres,
				paginas_requeridas);
		for (j = 0; j < paginas_requeridas; j++) {
			t_swap* swap = list_get(pagsAReemplazar, j);
			swap->bit_uso = 1;
			t_swap* swapReemplazado = list_replace(espacioTotal, swap->pagina, swap);
			free(swapReemplazado);

			t_proceso* proceso = malloc(sizeof(t_proceso));
			proceso->pagina = swap->pagina;
			proceso->pid = id_programa;
			list_add(listaDeProcesos, (void*) proceso);


			char* bufferDividido;
			bufferDividido = string_substring(programa,j*pagina_size,pagina_size);
			escribirArchivoBinarioEnPag(proceso->pagina,bufferDividido);
			free(bufferDividido);
			//free(proceso); //liberar proceso
		}
		for(j = (list_size(pagsAReemplazar) - 1); j >= 0; j--){
			list_remove(pagsAReemplazar, j);
		}
		list_destroy(pagsAReemplazar);
		for(j = (list_size(listaLibres) - 1); j >= 0; j--){
			list_remove(listaLibres, j);
		}
		list_destroy(listaLibres);
	}
}

void finalizar(int id_programa) { // testeado
	log_info(logger, "Finalizando proceso %d", id_programa);
	int j;
	int contPagDelProceso = 1;
	for (j = 0; j < list_size(listaDeProcesos); j++) {
		t_proceso* proceso = list_get(listaDeProcesos, j);
		if (proceso->pid == id_programa) {
			int paginaALiberar = proceso->pagina;
			escribir_pagina(id_programa,contPagDelProceso,paginaEnBlanco());
			contPagDelProceso++;
			list_remove(listaDeProcesos, j);
			t_swap* swap = list_get(espacioTotal, paginaALiberar);
			swap->bit_uso = 0;
			list_replace(espacioTotal, paginaALiberar, swap);
			j--;
			free(proceso);
		}

	}
	log_info(logger, "Proceso finalizado %d", id_programa);
}

int cant_pags_disponibles() {
	t_list* cantDisponibles = espaciosLibres();
	int cantidadDisponibles = list_size(cantDisponibles);
	int j;
	for(j = (list_size(cantDisponibles) - 1); j >= 0; j--){
		list_remove(cantDisponibles, j);
	}
	list_destroy(cantDisponibles);
	return cantidadDisponibles;
}

void leer_pagina(int id_programa, int num_pagina) {
	log_info(logger, "Leyendo pagina %d del programa %d", num_pagina, id_programa);
	int i;
	t_list* paginasDelProceso = list_create();
	int tamanioListaProcesos = list_size(listaDeProcesos);
	for (i = 0; i < tamanioListaProcesos; i++) {
		t_proceso* proceso = list_get(listaDeProcesos, i);
		if (proceso->pid == id_programa) {
			list_add(paginasDelProceso, proceso);
		}
	}
	if (list_get(paginasDelProceso, 0) == NULL) {
		log_info(logger, "No existe la pagina solicitada en la Swap");
	}
	for (i = 0; i < list_size(paginasDelProceso); i++) {
		if (i == num_pagina) {
			t_proceso* proceso = list_get(paginasDelProceso, i); //fijarse bien si es menos uno, creo que si
			char* paginaLeida = leerArchivoBinarioEnPagina(proceso->pagina);

			log_info(logger, "esto es lo leido %s\n", paginaLeida);

			enviar(socket_umc, paginaLeida, (pagina_size + 1));
			free(paginaLeida);

		} /*else {
		 log_info(logger, "No existe la pagina solicitada en la Swap");
		 }*/

	}
	for(i = (list_size(paginasDelProceso) - 1); i >= 0; i--){
		list_remove(paginasDelProceso, i);
	}
	list_destroy(paginasDelProceso);
	log_info(logger, "Leida la pagina %d del programa %d", num_pagina, id_programa);
}
//NO HABRIA QUE MANDARLE A LA UMC QUE ESTA TODO BIEN??
void escribir_pagina(int id_programa, int num_pagina, char* buffer) {
	/*int o =string_length(buffer);
	if(o != pagina_size){
		for(o = string_length(buffer); (o == pagina_size); o++){
			string_append(&buffer, " ");
		}

	}*/
	log_info(logger, "Escribiendo la pagina %d del programa %d", num_pagina, id_programa);
	int i;
	t_list* paginasDelProceso = list_create();
	int paginaAEscribir = -1;
	for (i = 0; i < list_size(listaDeProcesos); i++) { //Recorre la lista de procesos para ver si existe en la Swap
		t_proceso* proceso = list_get(listaDeProcesos, i); //Fallo acá
		if (proceso->pid == id_programa) {
			list_add(paginasDelProceso, proceso);
		}
	}
	if (list_get(paginasDelProceso, 0) == NULL) //si no encuentra el proceso
		log_info(logger, "No existe la pagina solicitada en la Swap");

	for (i = 0; i <= list_size(paginasDelProceso); i++) {
		if (i == num_pagina) {
			t_proceso* proceso = list_get(paginasDelProceso, i);
			paginaAEscribir = proceso->pagina;
		}
	}
	if (paginaAEscribir == -1)
		log_info(logger, "No existe la pagina solicitada en la Swap");
	else
		escribirArchivoBinarioEnPag(paginaAEscribir, buffer);
	log_info(logger, "Escrita la pagina %d del programa %d", num_pagina, id_programa);
	int q;
	for(q = 0; q < (list_size(paginasDelProceso)); q++){
		list_remove(paginasDelProceso, q);
	}
	list_destroy(paginasDelProceso);

}

char * paginaEnBlanco(){
	char * paginaBlanco = string_repeat(' ',pagina_size -1);

	return paginaBlanco;
}

void terminar(){

}
