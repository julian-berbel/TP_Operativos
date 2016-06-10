#include<commons/config.h>
#include<commons/log.h>
#include"umc.h"
#include "CUnit/Basic.h"
#include <string.h>


int main() {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_t thread_cpu;
	pthread_t thread_nucleo;


	abrirConfiguracion();
	log_info(logger, "Inicia proceso UMC");

	//comienzo inicializacion

	//array de disponibilidad de marcos
	marcos_libres = (int*) malloc(sizeof(int) * marcos);
	int k;
	for (k = 0; k < marcos; k++) { //lo inicializo en cero:testeado
		marcos_libres[k] = 0;
	}
	//array con la cantidad de paginas de cada proceso y procesos ocupados
	int p;
	for (p = 0; p < 50; p++) { //inicializo en cero
		cant_paginas_procesos[p] = 0;
		procesos_ocupados[p]=0;
	}

	//memoria
	memoria = (int *) malloc(marcos * marco_size);

	//TLB
	tlb = (TLB*) malloc(entradas_tlb * sizeof(TLB));
	int i;
	for (i = 0; i < entradas_tlb; i++) { //inicializo la tlb con idp en -1:testeado
				tlb[i].idp = -1;
	}
	//fin inicializacion

	socket_swap = crear_socket_cliente(ipSwap, puertoSwap);
	log_info(logger_pantalla, "UMC y Swap conectados");

	int socket_servidor = crear_socket_servidor(ipUmc, puertoUmc);
	int socket_cliente = recibirConexion(socket_servidor);
	char* mensaje;

	while (string_is_empty(mensaje = recibir_string_generico(socket_cliente)))
		;

		mensaje = recibir_string_generico(socket_cliente);
	if (strcmp(mensaje, "n")) {
		int *args_nucleo = malloc(sizeof(*args_nucleo));
		if (args_nucleo == NULL) {
			log_info(logger_pantalla,
					"No se pudo asignar memoria para un hilo del CPU");
			cerrar_todo();
		}
		*args_nucleo = socket_cliente;
		int r = pthread_create(&thread_nucleo, NULL, funcion_nucleo,
				args_nucleo);
		//Hay que hacer el pthread_join en algun lugar del main. Posiblemente en el cerrar_todo.
	} else if (strcmp(mensaje, "c")) {
		int *args_cpu = malloc(sizeof(*args_cpu));
		if (args_cpu == NULL) {
			cerrar_todo();
		}
		*args_cpu = socket_cliente;
		pthread_create(&thread_cpu, &attr, funcion_cpu, args_cpu);
	}
	//si es otro mensaje, llamar a la funcion procesarMensaje

	//Comienza el cierre del main.
	free(mensaje);
	close(socket_swap);
	close(socket_servidor);
	close(socket_cliente);
	pthread_attr_destroy(&attr);
	pthread_join(thread_nucleo, NULL);
	pthread_cancel(thread_cpu);
	cerrar_todo();
	return 0;
}

void abrirConfiguracion() {
	configuracionUMC = config_create(RUTA_CONFIG);
	ipUmc = config_get_string_value(configuracionUMC, "IP_UMC");
	puertoUmc = config_get_string_value(configuracionUMC, "PUERTO");
	ipSwap = config_get_string_value(configuracionUMC, "IP_SWAP");
	puertoSwap = config_get_string_value(configuracionUMC, "PUERTO_SWAP");
	marcos = config_get_int_value(configuracionUMC, "MARCOS");
	marco_size = config_get_int_value(configuracionUMC, "MARCO_SIZE");
	marco_x_proc = config_get_int_value(configuracionUMC, "MARCO_X_PROC");
	entradas_tlb = config_get_int_value(configuracionUMC, "ENTRADAS_TLB");
	tlb_habilitada = config_get_int_value(configuracionUMC, "TLB_HABILITADA");
	retardo = config_get_int_value(configuracionUMC, "RETARDO");
	logger = log_create(RUTA_LOG, "UMC", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "UMC", true, LOG_LEVEL_INFO);
}

void cerrar_todo() {
	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracionUMC);
}

void *funcion_nucleo(void *argumento) { // thread para nucleo
	int socket_nucleo = *((int *) argumento);
	char* mensaje = string_new();
	string_append(&mensaje, "okey\n");
	enviar_string(socket_nucleo, mensaje);
	free(mensaje);
	free(argumento);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}

void *funcion_cpu(void *argumento) {
	int socket_cpu = *((int *) argumento);
	char* mensaje = string_new();
	string_append(&mensaje, "okey\n");
	enviar_string(socket_cpu, mensaje);
	free(mensaje);
	close(socket_cpu);
	free(argumento);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

}

void inicializar(int id_programa, int paginas_requeridas, char* programa) {
	crear_tabla_de_paginas(id_programa, paginas_requeridas);//testeado

	//serializar inicializar
	//enviar_string(socket_swap, mensaje);
	//enviar a swap para que lo guarde

}

void finalizar(int id_programa) { //testeado
	memset(tabla_procesos[id_programa], '0', sizeof(tabla_paginas) * 20);
	//avisar al swap para que libere memoria
	procesos_ocupados[id_programa]=0;
}

void leer_pagina(int num_pagina, int offset, size_t t) {
	int marco = obtener_marco(id_proceso_activo, num_pagina);
	if (marco >= 0) { //si esta en MP
		modificar_bit_uso(id_proceso_activo, num_pagina);
		int contenido = memoria[marco * marco_size + offset]; //hacer funcion leer memoria, tener en cuenta el tamaño
		//enviar a la cpu el contenido
	} else { //si no esta en MP envio la solicitud a SWAP
		//serializar y enviar a swap
		//enviar_string(socket_swap, mensaje);
	}
}

void escribir_pagina(int num_pagina, int offset, size_t t, char *buffer) {
	int marco_libre=buscar_marco_libre();
	if(marco_libre!=-1){
		escribir_posicion_memoria(marco_libre * marco_size + offset, t, buffer);
		escribir_marco_en_TP(id_proceso_activo,num_pagina,marco_libre);
	}
}


void crear_tabla_de_paginas(int idp, int paginas_requeridas) { //testeado
	tabla_paginas tabla_paginas;
	//inicializo los marcos en -1
	int i;
	for (i = 0; i < paginas_requeridas; i++) {
		tabla_paginas.marco = -1;
		tabla_procesos[idp][i] = tabla_paginas;
	}
	cant_paginas_procesos[idp]=paginas_requeridas;
	procesos_ocupados[idp]=1;
}

int obtener_marco(int idp, int num_pagina) { //testeado
	int marco;
	if (tlb_habilitada) {
		marco = obtener_marco_tlb(idp, num_pagina);
		if (marco == -1) { //si no esta en la tlb
			marco = obtener_marco_tabla_paginas(idp, num_pagina);
			if (marco != -1) { //no estaba en la tlb pero si en la tabla de paginas(en MP)
				escribir_marco_en_tlb(idp, num_pagina, marco);
			}
		}
		aumentar_uso_tlb(idp, num_pagina);
	} else { //si no esta habilitada la tlb
		marco = obtener_marco_tabla_paginas(idp, num_pagina);
	}
	return marco;
}

int obtener_marco_tabla_paginas(int idp, int numero_pagina) { //testeado
	usleep(retardo * 1000);
	return tabla_procesos[idp][numero_pagina].marco;

}

int obtener_marco_tlb(int idp, int num_pagina) { //testeado
	int i = 0;
	while (i < entradas_tlb) {
		if (tlb[i].idp == idp && tlb[i].pagina == num_pagina) {
			return tlb[i].marco;
		}
		i++;
	}

	return -1;
}

void escribir_marco_en_TP(int idp, int pagina, int marco) { //al guardar en MP devuelve el marco en que se guardo y se guarda en la tabla de paginas del proceso
	tabla_procesos[idp][pagina].marco = marco;					//testeado
}

void escribir_marco_en_tlb(int idp, int num_pagina, int marco) {	//testeado
	int indice_libre = buscar_indice_libre_tlb();
	if (indice_libre != -1) { //si hay un indice libre
		tlb[indice_libre].idp = idp;
		tlb[indice_libre].pagina = num_pagina;
		tlb[indice_libre].marco = marco;
		tlb[indice_libre].uso = 0;
	} else { //no hay indice libre ->reemplazo con LRU
		int indice_menos_accedido = buscar_indice_menos_accedido_tlb();
		tlb[indice_menos_accedido].idp = idp;
		tlb[indice_menos_accedido].pagina = num_pagina;
		tlb[indice_menos_accedido].marco = marco;
		tlb[indice_menos_accedido].uso = 0;
	}
}

void marco_ocupado(int num_marco) { //testeado
	marcos_libres[num_marco] = 1;
}
void marco_desocupado(int num_marco) { //testeado
	marcos_libres[num_marco] = 0;
}

void cambiar_proceso_activo(int proceso) {
	int id_proceso_viejo = id_proceso_activo;
	id_proceso_activo = proceso;
	flush(id_proceso_viejo); //limpio lo que haya de ese proceso en la tlb
}
void modificar_bit_uso(int idp, int num_pagina) {
	tabla_procesos[idp][num_pagina].bit_uso = 1;
}
void modificar_bit_modificado(int idp, int num_pagina) {
	tabla_procesos[idp][num_pagina].modificado = 1;
}

void escribir_posicion_memoria(int posicion, size_t tamanio, char *buffer) {
	usleep(retardo * 1000);
	printf("o");
//escribir buffer
}
/*char* leer_posicion_memoria(int posicion, size_t tamanio){
 sleep(retardo/1000);
 */

int buscar_indice_libre_tlb() { //testeado
	int i;
	for (i = 0; i < entradas_tlb; i++) {
		if (tlb[i].idp == -1) {
			return i;
		}
	}
	return -1;
}

void aumentar_uso_tlb(int idp, int num_pagina) { //testeado
	int i;
	for (i = 0; i < entradas_tlb; i++) {
		if (tlb[i].idp != idp && tlb[0].pagina != num_pagina) {
			tlb[i].uso += 1;
		} else {
			tlb[i].uso = 0;
		}
	}
}

int buscar_indice_menos_accedido_tlb() { //testeado
	int indice, valormax = 0, i;
	for (i = 0; i < entradas_tlb; i++) {
		if (tlb[i].uso >= valormax) {
			valormax = tlb[i].uso;
			indice = i;
		}
	}
	return indice;
}

void flush(int idp) { //cuando cambia el proceso activo limpio el proceso viejo de la tlb
	int i = 0;
	for (i = 0; i < entradas_tlb; i++) {
		if (tlb[i].idp == idp) {
			tlb[i].idp = -1; //queda nuevamente como libre
		}
	}
}

//operaciones de consola
void modificar_retardo(int ret) { //testeado
	retardo = ret;
}

void dump_est_proceso(int idp) {//generar reporte en pantalla y un archivo sobre la tabla de paginas del proceso
	printf("Tabla de Paginas del Proceso: %d\n",idp);
	int pag;
	int cantidad_paginas=cant_paginas_procesos[idp];
	for (pag=0;pag<cantidad_paginas;pag++){
	printf("Pagina: %d, Marco: %d, Bit_uso: %d, Modificado: %d\n",pag, tabla_procesos[idp][pag].marco, tabla_procesos[idp][pag].bit_uso, tabla_procesos[idp][pag].modificado);
	}printf("\n");
}

void dump_est_gen() { //generar un reporte en pantalla y un archivo con las tablas de paginas de todos los procesos
	printf("Tablas de Paginas de todos los Procesos:\n\n");
	int proceso;
	for(proceso=0;proceso<50;proceso++){
		int proceso_existente=procesos_ocupados[proceso];
			if(proceso_existente){
				dump_est_proceso(proceso);
			}
		}
	}


void dump_cont_proceso(int idp) {
	//generar un reporte en pantalla y un archivo sobre los datos almacenados en memoria
	//de ese proceso
	printf("llegue a dump_cont_proceso");
}

void dump_cont_gen() {
	//generar un reporte en pantalla y un archivo sobre los datos almacenados en memoria
	//de todos los procesos
	printf("llegue a dump_cont_gen");
}

void flush_tlb() { //limpia completamente la tlb
	int i;
	for (i = 0; i < entradas_tlb; i++) {
		tlb[i].idp = -1;
	}
}

void flush_memory(int idp) { //marca todas las paginas del proceso como modificadas
	int cant_paginas=cant_paginas_procesos[idp];
	int i;
	for (i = 0; i < cant_paginas; i++) {
		tabla_procesos[idp][i].modificado = 1;
	}
}

void terminar(){

}

void reconocer_comando(char *comando, char* param){
	 if(!strcmp(comando,"retardo")){
		 modificar_retardo(atoi(param));
			}else if(!strcmp(comando,"dump_est")&& !strcmp(param,"gen")){
				dump_est_gen();
			}else if(!strcmp(comando,"dump_est")&& strcmp(param,"gen")){
				dump_est_proceso(atoi(param));
			}else if(!strcmp(comando,"dump_cont")&& !strcmp(param,"gen")){
				dump_cont_gen();
			}else if(!strcmp(comando,"dump_cont")&& strcmp(param,"gen")){
				dump_cont_proceso(atoi(param));
			}else if(!strcmp(comando,"flush")&& !strcmp(param,"tlb")){
				flush_tlb();
			}else if(!strcmp(comando,"flush_memory")){
				flush_memory(atoi(param));
			}else{
				printf("No existe el comando");
			}
}

int buscar_marco_libre(){
	int i;
	for(i=0;i<marcos;i++){
		if(marcos_libres[i]==0){
			return i;
		}
	}
return -1;
}


/*consola
	char* comando,*parametro,*comandoAEjecutar;
	comando = malloc(sizeof(char)*20);
	comandoAEjecutar = malloc(sizeof(char)*20);
	parametro = malloc(sizeof(char)*10);
	//while (1){
		scanf("%[^\n]",comando);
		comandoAEjecutar = strtok(comando," ");
		parametro = strtok(0," ");
		reconocer_comando(comandoAEjecutar,parametro);
	//}*/
