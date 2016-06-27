#include "swap.h"

int main() {
	abrirConfiguracion();
	crearArchivoBinario(nombre_data, pagina_size, cant_paginas);

	log_info(logger, "Inicia proceso Swap");

	int socket_servidor = crear_socket_servidor(ipSwap, puertoSwap);
	socket_umc = recibirConexion(socket_servidor);

	log_info(logger_pantalla, "Swap y UMC conectados");

	//inicializo las lista de utilizados en vacio porque no hay nada todavia? o como esta ahora???
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

	while(!flagTerminar){
	 mensaje = recibir(socket_umc);
	 if(!mensaje) break;
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
	retardo_compactacion = config_get_int_value(configuracion_swap, "RETARDO_COMPACTACION");
	retardo_acceso = config_get_int_value(configuracion_swap, "RETARDO_ACCESO");
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
		return 1;
	} else {
		return 0;
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
				j = j - 1;
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
	int cantidadEspaciosLibres = list_size(espaciosLibres());
	while(! (estanPaginasContinuas(espaciosLibres(),cantidadEspaciosLibres))){
		recorrerYModificarArchivoYListas();
	}

	usleep(retardo_compactacion * 1000);
}

void recorrerYModificarArchivoYListas(){

	char* paginaAMoverEnUso/*, paginaAMoverSinUso*/;
	int tamTotal = list_size(espacioTotal);
	int i;
	for(i=0 ; i < tamTotal ; i ++){ //Recorre la lista de espacioTotal para correr los espacios libres al final del archivo y de la lista
		t_swap* swap = list_get(espacioTotal,i);
		t_swap* swapSig = list_get(espacioTotal,i+1);
		if(swap->bit_uso == 0 && swapSig->bit_uso == 1){
			int j;
			int tamProcesos = list_size(listaDeProcesos);
			for(j = 0; j < tamProcesos; j++){ //Recorre lista de procesos para asignarle la nueva pagina
				t_proceso* proceso = list_get(listaDeProcesos, j);
				if(proceso->pagina == swapSig->pagina){
					proceso->pagina = swap->pagina;
					list_replace(listaDeProcesos, j, proceso);
					j = tamProcesos; //para que salga del for y no recorra toda la lista
				}
			}
			paginaAMoverEnUso = leerArchivoBinarioEnPagina(swapSig->pagina);
			//paginaAMoverSinUso = leerArchivoBinarioEnPagina()
			escribirArchivoBinarioEnPag(swap->pagina, paginaAMoverEnUso);
			swap->bit_uso = 1;
			swapSig->bit_uso = 0;
			list_replace(espacioTotal,i,swap);
			list_replace(espacioTotal, i+1, swapSig);

		}
	}
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
		t_list* pagsAReemplazar = paginasAReemplazar(listaLibres,
				paginas_requeridas);
		for (j = 0; j < paginas_requeridas; j++) {
			t_swap* swap = list_get(pagsAReemplazar, j);
			swap->bit_uso = 1;
			list_replace(espacioTotal, swap->pagina, swap);

			t_proceso* proceso = malloc(sizeof(t_proceso));
			proceso->pagina = swap->pagina;
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
	//recibir(socket_umc);
	int i;
	t_list* paginasDelProceso = list_create();
	int tamanioListaProcesos = list_size(listaDeProcesos);
	for (i = 0; i < tamanioListaProcesos; i++){
	t_proceso* proceso = list_get(listaDeProcesos,i);
		if(proceso->pid == id_programa){
			list_add(paginasDelProceso, proceso);
		}
	}
	if (list_get(paginasDelProceso, 0) == NULL){
					log_info(logger, "No existe la pagina solicitada en la Swap");
	}
		for (i = 0; i <= list_size(paginasDelProceso); i++) {
				if (i == num_pagina) {
					t_proceso* proceso = list_get(paginasDelProceso, i);
					char* paginaLeida = leerArchivoBinarioEnPagina(proceso->pagina);
					enviar(socket_umc, paginaLeida, pagina_size);

				}else{
					log_info(logger, "No existe la pagina solicitada en la Swap");
				}

	}

}
//NO HABRIA QUE MANDARLE A LA UMC QUE ESTA TODO BIEN??
void escribir_pagina(int id_programa, int num_pagina, char* buffer) {
	//recibir(socke_umc);
	int i;
	t_list* paginasDelProceso = list_create();
	int paginaAEscribir = -1;
	for (i = 0; i < list_size(listaDeProcesos); i++) { //Recorre la lista de procesos para ver si existe en la Swap
		t_proceso* proceso = list_get(listaDeProcesos, i); //Fallo acá
		if (proceso->pid == id_programa) {
			list_add(paginasDelProceso, proceso);
		}
	}
	if (list_get(paginasDelProceso, 0) == NULL) //si no encuentra el preoceso
		log_info(logger, "No existe la pagina solicitada en la Swap");

	for (i = 0; i <= list_size(paginasDelProceso); i++) {
		if (i == num_pagina) {
			t_proceso* proceso = list_get(paginasDelProceso, i);
			paginaAEscribir = proceso->pagina;
		}
	}
	if(paginaAEscribir == -1)
		log_info(logger, "No existe la pagina solicitada en la Swap");
	else
		escribirArchivoBinarioEnPag(paginaAEscribir,buffer);

}

void terminar() {

}
