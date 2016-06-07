#include <commons/config.h>
#include <commons/log.h>
#include "cpu.h"

//Empiezo a definir primitivas

t_puntero definirVariable(t_nombre_variable variable) {
	int nodos_stack = list_size(pcb_actual->indiceStack);
	int posicion_stack;
	int cantidad_variables;
	int cantidad_argumentos;
	int encontre_valor = 1;
	nodo_stack *nodo;
	pos_mem *posicion_memoria;
	t_variable *var;
	for(posicion_stack = (nodos_stack - 1); posicion_stack >= 0; posicion_stack--){
		nodo = list_get(pcb_actual->indiceStack, posicion_stack);
		cantidad_variables = list_size(nodo->vars);
		cantidad_argumentos = list_size(nodo->args);
		if(cantidad_variables != 0){
			var = list_get(nodo->vars, (cantidad_variables - 1));
			posicion_memoria = var->dir_var;
			posicion_stack = -1;
			encontre_valor = 0;
		} else if(cantidad_argumentos != 0){
			posicion_memoria = list_get(nodo->args, (cantidad_argumentos - 1));
			posicion_stack = -1;
			encontre_valor = 0;
		}
	}

	pos_mem* nueva_posicion_memoria = malloc(sizeof(pos_mem));
	t_variable *nueva_variable = malloc(sizeof(t_variable));
	nodo = list_get(pcb_actual->indiceStack, (nodos_stack - 1));
	if(encontre_valor == 0){
		if(((posicion_memoria->offset + posicion_memoria->size) + 4) > tamanio_pagina){
			nueva_posicion_memoria->pagina = (posicion_memoria->pagina + 1);
			nueva_posicion_memoria->offset = 0;
			nueva_posicion_memoria->size = 4;
			nueva_variable->nombre_var = variable;
			nueva_variable->dir_var = nueva_posicion_memoria;
			list_add(nodo->vars, nueva_variable);
		} else {
			nueva_posicion_memoria->pagina = posicion_memoria->pagina;
			nueva_posicion_memoria->offset = (posicion_memoria->offset + posicion_memoria->size);
			nueva_posicion_memoria->size = 4;
			nueva_variable->nombre_var = variable;
			nueva_variable->dir_var = nueva_posicion_memoria;
			list_add(nodo->vars, nueva_variable);
		}
	} else {
		if(tamanio_pagina < 4){
			printf("Tamaño de pagina menor a 4 bytes\n");
		} else {
			nueva_posicion_memoria->pagina = (pcb_actual->cantidadPaginas + 1);
			nueva_posicion_memoria->offset = 0;
			nueva_posicion_memoria->size = 4;
			nueva_variable->nombre_var = variable;
			nueva_variable->dir_var = nueva_posicion_memoria;
			list_add(nodo->vars, nueva_variable);
		}
	}

	int posicion_serializada = (nueva_posicion_memoria->pagina * tamanio_pagina) + nueva_posicion_memoria->offset;
	return posicion_serializada;
}

t_puntero obtenerPosicionVariable(t_nombre_variable variable) {
	printf("Obtener posicion de %c\n", variable);
	char *mensaje=string_new();
	string_append(&mensaje,"umc: busque la variable\n");
	enviar_string(socket_umc, mensaje);
	return POSICION_MEMORIA;
}

t_valor_variable dereferenciar(t_puntero puntero) {
	printf("Dereferenciar %d y su valor es: %d\n", puntero, CONTENIDO_VARIABLE);
	char *mensaje=string_new();
		string_append(&mensaje,"umc: dereferencie la variable\n");
		enviar_string(socket_umc, mensaje);
	return CONTENIDO_VARIABLE;
}

void asignar(t_puntero puntero, t_valor_variable variable) {
	printf("Asignando en %d el valor %d\n", puntero, variable);
	char *mensaje=string_new();
		string_append(&mensaje,"umc: asigne la variable\n");
		enviar_string(socket_umc, mensaje);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){
	return CONTENIDO_VARIABLE;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	return CONTENIDO_VARIABLE;
}

void irAlLabel(t_nombre_etiqueta etiqueta){

}

void llamarSinRetorno(t_nombre_etiqueta etiqueta){

}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){

}

void finalizar(){

}

void retornar(t_valor_variable retorno){

}

void imprimir(t_valor_variable valor) {
	printf("Imprimir %d\n", valor);
}

void imprimirTexto(char* texto) {
	printf("ImprimirTexto: %s", texto);
}

void wait(t_nombre_semaforo identificador_semaforo){

}

void signal(t_nombre_semaforo identificador_semaforo){

}

int main(){
	abrirConfiguracion();
	log_info(logger, "Inicia proceso CPU");
	int socket_nucleo = crear_socket_cliente(ipNucleo, puertoNucleo);
	log_info(logger_pantalla, "CPU y Nucleo conectados");
	socket_umc = crear_socket_cliente(ipUMC, puertoUMC);
	log_info(logger_pantalla, "CPU y UMC conectados");
	//Hay que pedirle el tamaño de pagina a la UMC para definirlo
	tamanio_pagina = 10;//Definir correctamente cuando lo sepa
	char * mensaje;
	while(string_is_empty(mensaje = recibir_string_generico(socket_nucleo)));
	char * mensaje_logger = string_new();
	string_append(&mensaje_logger, "Pase por la CPU - ");
	string_append(&mensaje_logger, mensaje);
	log_info(logger_pantalla, mensaje_logger);
	free(mensaje_logger);

	enviar_string(socket_umc, mensaje);

	close(socket_nucleo);
	free(mensaje);
	cerrar_todo();

	return 0;
}

void abrirConfiguracion(){
	configuracionCPU = config_create(RUTA_CONFIG);
	ipNucleo = config_get_string_value(configuracionCPU, "IP_NUCLEO");
	puertoNucleo = config_get_string_value(configuracionCPU, "PUERTO_NUCLEO");
	ipUMC = config_get_string_value(configuracionCPU, "IP_UMC");
	puertoUMC = config_get_string_value(configuracionCPU, "PUERTO_UMC");
	logger = log_create(RUTA_LOG, "CPU", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "CPU", true, LOG_LEVEL_INFO);

	printf("%s\n", ipNucleo);
	printf("%s\n", puertoNucleo);
	printf("%s\n", ipUMC);
	printf("%s\n", puertoUMC);

}

void cerrar_todo(){
	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracionCPU);
}

char* obtener_instruccion(t_PCB * pcb){
	int program_counter = pcb->programCounter;
	int byte_inicio_instruccion = pcb->indiceCodigo[program_counter][0];
	int bytes_tamanio_instruccion = pcb->indiceCodigo[program_counter][1];
	int num_pagina = byte_inicio_instruccion / tamanio_pagina;
	int offset = byte_inicio_instruccion - (num_pagina * tamanio_pagina);
	char* instruccion;
	char* continuacion_instruccion;
	int bytes_a_leer_primera_pagina;
	if (bytes_tamanio_instruccion > (tamanio_pagina * 2)){
		printf("El tamanio de la instruccion es mayor al tamanio de pagina\n");
	}
	if ((offset + bytes_tamanio_instruccion) < tamanio_pagina){
		instruccion = pedir_bytes_umc(num_pagina, offset, bytes_tamanio_instruccion);
	} else {
		bytes_a_leer_primera_pagina = tamanio_pagina - offset;
		instruccion = pedir_bytes_umc(num_pagina, offset, bytes_a_leer_primera_pagina);
		continuacion_instruccion = pedir_bytes_umc((num_pagina + 1), 0, (bytes_tamanio_instruccion - bytes_a_leer_primera_pagina));
		string_append(&instruccion, continuacion_instruccion);
		free(continuacion_instruccion);
	}
	return instruccion;
	//Una vez que se usa la instruccion hay que hacer free
}


char* pedir_bytes_umc(int num_pagina, int offset, int tamanio){
	char* instruccion;
	void* pedido;
	int tamanioSerializacion = serializarSolicitar(num_pagina, offset, tamanio, &pedido);
	enviar(socket_umc, pedido, tamanioSerializacion);
	char* recibido = recibir(socket_umc);
	string_append(&instruccion, recibido);
	free(recibido);
	return instruccion;
}
