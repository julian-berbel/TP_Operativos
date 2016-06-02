#include<commons/config.h>
#include<commons/log.h>
#include"umc.h"
#include "CUnit/Basic.h"

int main() {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_t thread_cpu;
	pthread_t thread_nucleo;

	abrirConfiguracion();
	log_info(logger, "Inicia proceso UMC");

	//array de disponibilidad de marcos
	marcos_libres = (int*) malloc(sizeof(int) * marcos);
	memset(marcos_libres, '0', sizeof(int));

	//memoria
	memoria = (int *) malloc(marcos * marco_size);

	//TLB
	tlb = (TLB*) malloc(entradas_tlb * sizeof(TLB));
	//inicializo la tlb con paginas en -1
	int i;
	for (i = 0; i < entradas_tlb; i++) {
		tlb[i].pagina = -1;
	}

	socket_swap = crear_socket_cliente(ipSwap, puertoSwap);
	log_info(logger_pantalla, "UMC y Swap conectados");

	int socket_servidor = crear_socket_servidor(ipUmc, puertoUmc);
	int socket_cliente = recibirConexion(socket_servidor);
	char* mensaje;
	while (string_is_empty(mensaje = recibir_string_generico(socket_cliente)))
		;
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
	} else if (strcmp(mensaje, "inicializar")) { //en el mensaje debe recibir los parametros necesarios
		inicializar(4, 5, "lala"); //id_programa:4, paginas_requeridas:5
	} else if (strcmp(mensaje, "finalizar")) {
		finalizar(4); //id_programa:4
	} else if (strcmp(mensaje, "leer_pagina")) {
		int contenido = leer_pagina(3, 50, 100); //nro_pagina:3,offset:50,tamaño:100
		if (contenido != -1000) { //como saber si no lo encontro?
			//enviar a la cpu el contenido
		} else {
			//esperar respuesta de la swap y enviar al cpu
		}
	} else if (strcmp(mensaje, "escribir_pagina")) {
		escribir_pagina(5, 40, 80, "hola"); //nro_pagina:5,offset:40,tamaño:80,bufer:"hola"
	} else {
		printf("%s", mensaje); //para los mensajes de las primitivas checkpoint 2
	}
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

	/*printf("%s\n", ipUmc);
	 printf("%s\n", puertoUmc);
	 printf("%s\n", ipSwap);
	 printf("%s\n", puertoSwap);
	 printf("%d\n", marcos);
	 printf("%d\n", marco_size);
	 printf("%d\n", marco_x_proc);
	 printf("%d\n", entradas_tlb);
	 printf("%d\n", tlb_habilitada);
	 printf("%d\n", retardo);*/
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
	crear_tabla_de_paginas(id_programa, paginas_requeridas);
	char* mensaje = serializar2(1, id_programa, paginas_requeridas, programa); //1=inicializar
	enviar_string(socket_swap, mensaje);
	//enviar a swap para que lo guarde
}
void finalizar(int id_programa) {
	free(tabla_procesos[id_programa]);
	//avisar al swap para que libere memoria

}

int leer_pagina(int num_pagina, int offset, size_t t) {
	int marco = obtener_marco(num_pagina);
	if (marco >= 0) { //si esta en MP
		modificar_bit_uso(id_proceso_activo, num_pagina);
		return memoria[marco * marco_size + offset]; //hacer funcion leer memoria, tener en cuenta el tamaño
	} else { //si no esta en MP envio la solicitud a SWAP
		char* mensaje = serializar1(3, num_pagina, offset, t); //3=leer_pagina
		enviar_string(socket_swap, mensaje);
		return -1; //si devuelve -1 tengo que esperar al swap
	}
}

void escribir_pagina(int num_pagina, int offset, size_t t, char *buffer) {
	int marco = obtener_marco(num_pagina);
	if (marco >= 0) { //esta en MP
		escribir_posicion_memoria(marco * marco_size + offset, t, buffer);
	}
}

void crear_tabla_de_paginas(int idp, int paginas_requeridas) {
	tabla_paginas tabla_paginas[paginas_requeridas];
	//la inicializo en -1
	int i;
	for (i = 0; i < paginas_requeridas; i++)
		tabla_paginas[i].marco = -1;
	tabla_procesos[idp] = tabla_paginas;
}

int obtener_marco(int num_pagina) {
	int marco;
	if (tlb_habilitada) {
		marco = obtener_marco_tlb(num_pagina);
		if (marco == -1) { //si no esta en la tlb
			marco = obtener_marco_tabla_paginas(num_pagina);
			if (marco != -1) { //no estaba en la tlb pero si en la tabla de paginas(en MP)
				guardar_en_tlb(num_pagina, marco);
			}
		}
	}
else { //si no esta habilitada la tlb
	marco = obtener_marco_tabla_paginas(num_pagina);
}
return marco;
}

int obtener_marco_tabla_paginas(int numero_pagina) {
sleep(retardo);
return tabla_procesos[id_proceso_activo][numero_pagina].marco;

}

void escribir_marco_en_TP(int idp, int pagina, int marco) { //al guardar en MP devuelve el marco en que se guardo y se guarda en la tabla de paginas del proceso
tabla_procesos[idp]->marco = marco;
}

void marco_ocupado(int num_marco) {
marcos_libres[num_marco] = 1;
}
void marco_desocupado(int num_marco) {
marcos_libres[num_marco] = 0;
}
void modificar_retardo(int ret) {
retardo = ret;
}

void cambiar_proceso_activo(int proceso) {
id_proceso_activo = proceso;
}
void modificar_bit_uso(int idp, int num_pagina) {
tabla_procesos[idp]->bit_uso = 1;
}
void modificar_bit_modificado(int idp, int num_pagina) {
tabla_procesos[idp]->modificado = 1;
}

char* serializar1(int operacion, int num_pagina, int offset, size_t t) {
char* mensaje = string_new();
string_append(&mensaje, (char*) operacion);
string_append(&mensaje, (char*) '|');
string_append(&mensaje, (char*) num_pagina);
string_append(&mensaje, (char*) '|');
string_append(&mensaje, (char*) t);
return mensaje;
}
char* serializar2(int operacion, int id_programa, int paginas_requeridas,
	char* programa) {
char* mensaje = string_new();
string_append(&mensaje, (char*) operacion);
string_append(&mensaje, (char*) '|');
string_append(&mensaje, (char*) id_programa);
string_append(&mensaje, (char*) '|');
string_append(&mensaje, (char*) paginas_requeridas);
string_append(&mensaje, (char*) '|');
string_append(&mensaje, programa);
return mensaje;
}
int obtener_marco_tlb(int num_pagina) {
int i = 0;
while (i < entradas_tlb) {
	if (tlb[i].pagina == num_pagina) {
		return tlb[i].marco;
	}
	i++;
}
return -1;
}
void escribir_posicion_memoria(int posicion, size_t tamanio, char *buffer) {
sleep(retardo);
//ver como escribo el buffer
}
/*char* leer_posicion_memoria(int posicion, size_t tamanio){

 */
void guardar_en_tlb(int num_pagina, int marco) {
int indice_libre = buscar_indice_libre_tlb();
if (indice_libre != -1) { //si hay un indice libre
	tlb[indice_libre].pagina = num_pagina;
	tlb[indice_libre].marco = marco;
	tlb[indice_libre].uso = 0;
} else { //no hay indice libre ->reemplazo con LRU

}
}

int buscar_indice_libre_tlb() {
int i;
for (i = 0; i < entradas_tlb; i++) {
	if (tlb[i].pagina == -1) {
		return i;
	}
}
return -1;
}
