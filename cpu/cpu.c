#include <commons/config.h>
#include <commons/log.h>
#include "cpu.h"

//Definicion de primitivas

t_puntero definirVariable(t_nombre_variable variable) {
	int nodos_stack = list_size(pcb_actual->indiceStack);
	int posicion_stack;
	int cantidad_variables;
	int cantidad_argumentos;
	int encontre_valor = 1;
//	char tipo;
	nodo_stack *nodo;
	pos_mem *posicion_memoria;
	t_variable *var;
	pos_mem* nueva_posicion_memoria;
	t_variable *nueva_variable;
	if(pcb_finalizar == 1){
		return 0;
	}
	if(nodos_stack == 0){
		nodo_stack *nodo = malloc(sizeof(nodo_stack));
		nodo->args = list_create();
		nodo->vars = list_create();
		nodo->dir_retorno = 0;
		pos_mem *retorno = malloc(sizeof(pos_mem));
		retorno->pagina = 0;
		retorno->offset = 0;
		retorno->size = 0;
		nodo->var_retorno = retorno;
		list_add(pcb_actual->indiceStack, nodo);
	}
	nodos_stack = list_size(pcb_actual->indiceStack);
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

	if((variable >= '0') && (variable <= '9')){
		nueva_posicion_memoria = malloc(sizeof(pos_mem));
		nodo = list_get(pcb_actual->indiceStack, (nodos_stack - 1));
		if(encontre_valor == 0){
			if(((posicion_memoria->offset + posicion_memoria->size) + 4) > tamanio_pagina){
				nueva_posicion_memoria->pagina = (posicion_memoria->pagina + 1);
				nueva_posicion_memoria->offset = 0;
				nueva_posicion_memoria->size = 4;
				if(nueva_posicion_memoria->pagina >= pcb_actual->cantidadPaginas){
					stackOverflow();
				} else {
					list_add(nodo->args, nueva_posicion_memoria);
				}
			} else {
				nueva_posicion_memoria->pagina = posicion_memoria->pagina;
				nueva_posicion_memoria->offset = (posicion_memoria->offset + posicion_memoria->size);
				nueva_posicion_memoria->size = 4;
				if(nueva_posicion_memoria->pagina >= pcb_actual->cantidadPaginas){
					stackOverflow();
				} else {
					list_add(nodo->args, nueva_posicion_memoria);
				}
			}
		} else {
			if(tamanio_pagina < 4){
				printf("Tamaño de pagina menor a 4 bytes\n");
			} else {
				nueva_posicion_memoria->pagina = (pcb_actual->cantidadPaginas - tamanio_stack);
				nueva_posicion_memoria->offset = 0;
				nueva_posicion_memoria->size = 4;
				if(nueva_posicion_memoria->pagina >= pcb_actual->cantidadPaginas){
					stackOverflow();
				} else {
					list_add(nodo->args, nueva_posicion_memoria);
				}
			}
		}
	} else {
		nueva_posicion_memoria = malloc(sizeof(pos_mem));
		nueva_variable = malloc(sizeof(t_variable));
		nodo = list_get(pcb_actual->indiceStack, (nodos_stack - 1));
		if(encontre_valor == 0){
			if(((posicion_memoria->offset + posicion_memoria->size) + 4) > tamanio_pagina){
				nueva_posicion_memoria->pagina = (posicion_memoria->pagina + 1);
				nueva_posicion_memoria->offset = 0;
				nueva_posicion_memoria->size = 4;
				nueva_variable->nombre_var = variable;
				nueva_variable->dir_var = nueva_posicion_memoria;
				if(nueva_posicion_memoria->pagina >= pcb_actual->cantidadPaginas){
					stackOverflow();
				} else {
					list_add(nodo->vars, nueva_variable);
				}
			} else {
				nueva_posicion_memoria->pagina = posicion_memoria->pagina;
				nueva_posicion_memoria->offset = (posicion_memoria->offset + posicion_memoria->size);
				nueva_posicion_memoria->size = 4;
				nueva_variable->nombre_var = variable;
				nueva_variable->dir_var = nueva_posicion_memoria;
				if(nueva_posicion_memoria->pagina >= pcb_actual->cantidadPaginas){
					stackOverflow();
				} else {
					list_add(nodo->vars, nueva_variable);
				}
			}
		} else {
			if(tamanio_pagina < 4){
				printf("Tamaño de pagina menor a 4 bytes\n");
			} else {
				nueva_posicion_memoria->pagina = (pcb_actual->cantidadPaginas - tamanio_stack);
				nueva_posicion_memoria->offset = 0;
				nueva_posicion_memoria->size = 4;
				nueva_variable->nombre_var = variable;
				nueva_variable->dir_var = nueva_posicion_memoria;
				if(nueva_posicion_memoria->pagina >= pcb_actual->cantidadPaginas){
					stackOverflow();
				} else {
					list_add(nodo->vars, nueva_variable);
				}
			}
		}
	}

	int posicion_serializada = (nueva_posicion_memoria->pagina * tamanio_pagina) + nueva_posicion_memoria->offset;

	return posicion_serializada;
}

t_puntero obtenerPosicionVariable(t_nombre_variable variable) {
	log_info(logger, "Obteniendo la posicion de la variable: %c", variable);
	int nodos_stack = list_size(pcb_actual->indiceStack);
	int cantidad_variables;
	int i;
	int encontre_valor = 1;
	nodo_stack *nodo;
	pos_mem *posicion_memoria;
	t_variable *var;
	nodo = list_get(pcb_actual->indiceStack, (nodos_stack - 1));
	if((variable >= '0') && (variable <= '9')){
		int variable_int = variable - '0';
		posicion_memoria = list_get(nodo->args, variable_int);
		if(posicion_memoria != NULL){
			encontre_valor = 0;
		}
	} else {
		cantidad_variables = list_size(nodo->vars);
		for(i = 0; i < cantidad_variables; i++){
			var = list_get(nodo->vars, i);
			if(var->nombre_var == variable){
				posicion_memoria = var->dir_var;
				encontre_valor = 0;
			}
		}
	}

	if(encontre_valor == 1){
		printf("ObtenerPosicionVariable: No se encontro variable o argumento\n");
		//Escribir en log
		return -1;
	}
	int posicion_serializada = (posicion_memoria->pagina * tamanio_pagina) + posicion_memoria->offset;
	return posicion_serializada;
}

t_valor_variable dereferenciar(t_puntero puntero) {
	int num_pagina = puntero / tamanio_pagina;
	int offset = puntero - (num_pagina * tamanio_pagina);
	char *valor_variable_char = pedir_bytes_umc(num_pagina, offset, 4);
	log_info(logger, "Valor Obtenido de la umc: %s", valor_variable_char);
	char *ptr;
	int valor_variable = strtol(valor_variable_char, &ptr, 10);
	free(valor_variable_char);
	log_info(logger, "dereferenciar: Valor Obtenido: %d", valor_variable);
	return valor_variable;
}

void asignar(t_puntero puntero, t_valor_variable variable) {
	int num_pagina = puntero / tamanio_pagina;
	int offset = puntero - (num_pagina * tamanio_pagina);
	char *valor_variable = string_itoa(variable);
	enviar_bytes_umc(num_pagina, offset, 4, valor_variable);
	log_info(logger, "asignar: Valor a Asignar: %s", valor_variable);
	free(valor_variable);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){
	char** string_cortado = string_split(variable, "\n");
	char* variable_con_formato = string_new();
	string_append(&variable_con_formato, "!");
	string_append(&variable_con_formato, string_cortado[0]);
	void* variable_serializada;
	int tamanioMensaje = serializarObtenerValor(variable_con_formato, &variable_serializada);
	enviar(socket_nucleo, variable_serializada, tamanioMensaje);
	free(variable_con_formato);
	free(variable_serializada);
	int* valor_variable_recibida = malloc(sizeof(int));
	valor_variable_recibida = (int*) recibir(socket_nucleo);
	int valor_variable = *valor_variable_recibida;
	free(valor_variable_recibida);
	log_info(logger, "Valor convertido del nucleo: %d", valor_variable);
	int i = 0;
	while(string_cortado[i] != NULL){
		free(string_cortado[i]);
		i++;
	}
	free(string_cortado);
	return valor_variable;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	char** string_cortado = string_split(variable, "\n");
	char* variable_con_formato = string_new();
	string_append(&variable_con_formato, "!");
	string_append(&variable_con_formato, string_cortado[0]);
	void* variable_serializada;
	int tamanioMensaje = serializarGrabarValor(variable_con_formato, valor, &variable_serializada);
	enviar(socket_nucleo, variable_serializada, tamanioMensaje);
	free(variable_con_formato);
	free(variable_serializada);
	int i = 0;
	while(string_cortado[i] != NULL){
		free(string_cortado[i]);
		i++;
	}
	free(string_cortado);
	return valor;
}

void irAlLabel(t_nombre_etiqueta etiqueta){
	char** string_cortado = string_split(etiqueta, "\n");
	int program_counter = metadata_buscar_etiqueta(string_cortado[0], pcb_actual->indiceEtiquetas, pcb_actual->tamanioIndiceEtiquetas);
	if(program_counter == -1){
		log_info(logger_pantalla, "No se encontro la etiqueta: %s en el indice de etiquetas", string_cortado[0]);
	} else {
		pcb_actual->programCounter = (program_counter - 1);
		log_info(logger, "Program Counter, despues de etiqueta: %d", pcb_actual->programCounter);
	}
	int i = 0;
	while(string_cortado[i] != NULL){
		free(string_cortado[i]);
		i++;
	}
	free(string_cortado);
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta){

}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){
	nodo_stack *nodo = malloc(sizeof(nodo_stack));
	nodo->args = list_create();
	nodo->vars = list_create();
	nodo->dir_retorno = (pcb_actual->programCounter);//Puede ser programCounter + 1
	int num_pagina = donde_retornar / tamanio_pagina;
	int offset = donde_retornar - (num_pagina * tamanio_pagina);
	pos_mem *retorno = malloc(sizeof(pos_mem));
	retorno->pagina = num_pagina;
	retorno->offset = offset;
	retorno->size = 4;
	nodo->var_retorno = retorno;
	list_add(pcb_actual->indiceStack, nodo);
	char** string_cortado = string_split(etiqueta, "\n");
	int program_counter = metadata_buscar_etiqueta(string_cortado[0], pcb_actual->indiceEtiquetas, pcb_actual->tamanioIndiceEtiquetas);
	if(program_counter == -1){
		printf("No se encontro la funcion %s en el indice de etiquetas\n", string_cortado[0]);
	} else {
		pcb_actual->programCounter = (program_counter - 1);
	}
	int i = 0;
	while(string_cortado[i] != NULL){
		free(string_cortado[i]);
		i++;
	}
	free(string_cortado);
}

void finalizar(){
	pcb_finalizar = 1;
	void* programa_terminado_serializado;
	int tamanioMensaje = serializarProgramaTerminado(&programa_terminado_serializado);
	enviar(socket_nucleo, programa_terminado_serializado, tamanioMensaje);
	free(programa_terminado_serializado);
}

void retornar(t_valor_variable retorno){
	nodo_stack *nodo;
	int cantidad_nodos = list_size(pcb_actual->indiceStack);
	nodo = list_remove(pcb_actual->indiceStack, (cantidad_nodos - 1));
	pos_mem *posicion_memoria;
	posicion_memoria = nodo->var_retorno;
	int num_pagina = posicion_memoria->pagina;
	int offset = posicion_memoria->offset;
	char *valor_variable = string_itoa(retorno);
	enviar_bytes_umc(num_pagina, offset, 4, valor_variable);
	free(valor_variable);
	pcb_actual->programCounter = nodo->dir_retorno;// Puede ser la dir_retorno + 1

	//Elimino el nodo de la lista
	int cantidad_argumentos;
	int cantidad_variables;
	t_variable *var;
	cantidad_argumentos = list_size(nodo->args);
	while(cantidad_argumentos != 0){
		posicion_memoria = list_remove(nodo->args, (cantidad_argumentos - 1));
		free(posicion_memoria);
		cantidad_argumentos = list_size(nodo->args);
	}
	list_destroy(nodo->args);
	cantidad_variables = list_size(nodo->vars);
	while(cantidad_variables != 0){
		var = list_remove(nodo->vars, (cantidad_variables - 1));
		free(var->dir_var);
		free(var);
		cantidad_variables = list_size(nodo->vars);
	}
	list_destroy(nodo->vars);
	free(nodo->var_retorno);
	free(nodo);
}

void imprimir(t_valor_variable valor) {
	char *valor_variable = string_itoa(valor);
	void* texto_serializado;
	int tamanioMensaje = serializarImprimir(valor_variable, &texto_serializado);
	enviar(socket_nucleo, texto_serializado, tamanioMensaje);
	free(texto_serializado);
	free(valor_variable);
}

void imprimirTexto(char* texto) {
	void* texto_serializado;
	int tamanioMensaje = serializarImprimir(texto, &texto_serializado);
	enviar(socket_nucleo, texto_serializado, tamanioMensaje);
	free(texto_serializado);
}

void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo){
	void* entradaSalida_serializado;
	int tamanioMensaje = serializarEntradaSalida(dispositivo, tiempo, &entradaSalida_serializado);
	enviar(socket_nucleo, entradaSalida_serializado, tamanioMensaje);
	free(entradaSalida_serializado);
	pcb_bloqueado = 1;
}

void wait(t_nombre_semaforo identificador_semaforo){
	char** string_cortado = string_split(identificador_semaforo, "\n");
	log_info(logger, "Semaforo a bajar: %s", string_cortado[0]);
	void* wait_serializado;
	int tamanioMensaje = serializarWait(string_cortado[0], &wait_serializado);
	enviar(socket_nucleo, wait_serializado, tamanioMensaje);
	free(wait_serializado);
	char* mensaje = recibir_string_generico(socket_nucleo);
	if(strcmp(mensaje, "dale para adelante!") != 0){
		pcb_bloqueado = 1;
		log_info(logger, "pid: %d bloqueado por semaforo: %s", pcb_actual->pid, string_cortado[0]);
	}
	int i = 0;
	while(string_cortado[i] != NULL){
		free(string_cortado[i]);
		i++;
	}
	free(string_cortado);
	free(mensaje);
}

void signal_primitiva(t_nombre_semaforo identificador_semaforo){
	char** string_cortado = string_split(identificador_semaforo, "\n");
	log_info(logger, "Semaforo a subir: %s", string_cortado[0]);
	void* signal_serializado;
	int tamanioMensaje = serializarSignal(string_cortado[0], &signal_serializado);
	enviar(socket_nucleo, signal_serializado, tamanioMensaje);
	free(signal_serializado);
	int i = 0;
	while(string_cortado[i] != NULL){
		free(string_cortado[i]);
		i++;
	}
	free(string_cortado);
}

void cerrarCPU(int n){
	if(n == SIGUSR1){
		void* mensaje;
		int tamanioMensaje = serializarCerrarCPU(&mensaje);
		enviar(socket_nucleo, mensaje, tamanioMensaje);
		free(mensaje);
		flagTerminar = 1;
	}
}

int main(int cantidadArgumentos, char* argumentos[]){
	abrirConfiguracion(argumentos[1]);
	signal(SIGUSR1, cerrarCPU);
	log_info(logger, "Inicia proceso CPU");
	socket_nucleo = crear_socket_cliente(ipNucleo, puertoNucleo);
	log_info(logger_pantalla, "CPU y Nucleo conectados");
	void* mensaje = recibir(socket_nucleo);
	tamanio_stack = *(int*)mensaje;
	free(mensaje);
	socket_umc = crear_socket_cliente(ipUMC, puertoUMC);
	log_info(logger_pantalla, "CPU y UMC conectados");
	mensaje = recibir(socket_umc);
	tamanio_pagina = *(int*)mensaje;
	free(mensaje);
	pcb_actual = malloc(sizeof(t_PCB));//Fake pcb para arrancar
	pcb_actual->pid = 151515;
	pcb_actual->programCounter = 0;
	pcb_actual->cantidadPaginas = 2;
	pcb_actual->cantidadInstrucciones = 0;
	pcb_actual->indiceCodigo = malloc(sizeof(int));
	pcb_actual->tamanioIndiceEtiquetas = 1;
	pcb_actual->indiceEtiquetas = malloc(50);
	pcb_actual->indiceStack = list_create();

	while(!flagTerminar){
		mensaje = recibir(socket_nucleo);
		if(!mensaje)break;
		if((strcmp(mensaje, "cerrate") == 0)){
			cpu_ocupada = 0;
		}
		if((strcmp(mensaje, "cerrate") != 0)){
			procesarMensaje(mensaje, NULL);
		}
	}

	if(cpu_ocupada){
		mensaje = recibir(socket_nucleo);
		procesarMensaje(mensaje, NULL);
	}

	if(!cpu_ocupada){
		log_info(logger, "La cpu esta libre antes de cerrarse");
	}
	if(cpu_ocupada){
		log_info(logger, "La cpu sigue ocupada antes de cerrarse");
	}

	cerrar_todo();

	return 0;
}

void abrirConfiguracion(char* ruta){
	configuracionCPU = config_create(ruta);
	ipNucleo = config_get_string_value(configuracionCPU, "IP_NUCLEO");
	puertoNucleo = config_get_string_value(configuracionCPU, "PUERTO_NUCLEO");
	ipUMC = config_get_string_value(configuracionCPU, "IP_UMC");
	puertoUMC = config_get_string_value(configuracionCPU, "PUERTO_UMC");
	char* ruta_log = devolver_ruta_log();
	logger = log_create(ruta_log, "CPU", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(ruta_log, "CPU", true, LOG_LEVEL_INFO);
	free(ruta_log);

	printf("%s\n", ipNucleo);
	printf("%s\n", puertoNucleo);
	printf("%s\n", ipUMC);
	printf("%s\n", puertoUMC);

}

void cerrar_todo(){
	void* mensaje;
	int tamanioMensaje = serializarCerrarCPU(&mensaje);
	enviar(socket_nucleo, mensaje, tamanioMensaje);
	free(mensaje);
	shutdown(socket_nucleo, 0);
	close(socket_nucleo);
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
		log_info(logger, "Primer parte de instruccion: %s", instruccion);
		if((bytes_tamanio_instruccion - bytes_a_leer_primera_pagina) > 0){
			continuacion_instruccion = pedir_bytes_umc((num_pagina + 1), 0, (bytes_tamanio_instruccion - bytes_a_leer_primera_pagina));
			log_info(logger, "Continuacion ejecucion: %s", continuacion_instruccion);
			string_append(&instruccion, continuacion_instruccion);
			free(continuacion_instruccion);
		}else{
			log_info(logger, "La continuacion de la instruccion es 0. Ni la leo");
		}
	}
	char** string_cortado = string_split(instruccion, "\n");
	free(instruccion);
	instruccion = string_new();
	string_append(&instruccion, string_cortado[0]);
	log_info(logger, "Instruccion obtenida: %s", instruccion);
	int i = 0;
	while(string_cortado[i] != NULL){
		free(string_cortado[i]);
		i++;
	}
	free(string_cortado);
	return instruccion;
	//Una vez que se usa la instruccion hay que hacer free
}


char* pedir_bytes_umc(int num_pagina, int offset, int tamanio){
	char* instruccion = string_new();
	void* pedido;
	int tamanioSerializacion = serializarSolicitar(num_pagina, offset, tamanio, &pedido);
	enviar(socket_umc, pedido, tamanioSerializacion);
	char* recibido = recibir(socket_umc);
	string_append(&instruccion, recibido);
	free(recibido);
	free(pedido);
	return instruccion;
}

void enviar_bytes_umc(int num_pagina, int offset, int tamanio, char* buffer){
	void* envio_serializado;
	int tamanioSerializacion = serializarAlmacenar(num_pagina, offset, tamanio, buffer, &envio_serializado);
	enviar(socket_umc, envio_serializado, tamanioSerializacion);
	free(envio_serializado);
}

void cargarPCB(t_PCB* pcb, int quantum){
	pcb_destroy(pcb_actual);
	pcb_actual = pcb;
	cpu_ocupada = 1;
	quantum_definido = quantum;
	pcb_bloqueado = 0;
	pcb_finalizar = 0;
	void* pid_serializado;
	int tamanioMensaje = serializarCambioDeProcesoActivo(pcb_actual->pid, &pid_serializado);
	enviar(socket_umc, pid_serializado, tamanioMensaje);
	free(pid_serializado);
	ciclosDeQuantum();
}

void ciclosDeQuantum(){
	int i = 0;
	while((i < quantum_definido) && (pcb_bloqueado == 0) && (pcb_finalizar == 0)){
		ejecutarInstruccion();
		i++;
	}
	if((i == quantum_definido) && (pcb_bloqueado == 0) && (pcb_finalizar == 0)){
		void* serializacionQuantum;
		int tamanioMensaje = serializarQuantumTerminado(&serializacionQuantum);
		enviar(socket_nucleo, serializacionQuantum, tamanioMensaje);
		free(serializacionQuantum);
	}
}

void ejecutarInstruccion(){
	char* instruccion = obtener_instruccion(pcb_actual);
	analizadorLinea(instruccion, &functions, &kernel_functions);
	free(instruccion);
	pcb_actual->programCounter = pcb_actual->programCounter + 1;
}

void terminar(){
	// terminar lo recibe cuando se cierra el nucleo.
	log_info(logger, "Se envio desde el nucleo la señal para cerrarse. Comenzando cierre del CPU");
	flagTerminar = 1;
}

void continuarEjecucion(int quantum){
	quantum_definido = quantum;
	ciclosDeQuantum();
}

void desalojar(){
	log_info(logger, "Desalojando pid: %d", pcb_actual->pid);
	void* mensaje = NULL;
	int tamanioMensaje = serializarPCB(pcb_actual, &mensaje);
	enviar(socket_nucleo, mensaje, tamanioMensaje);
	free(mensaje);
	cpu_ocupada = 0;
	//pcb_destroy(pcb_actual);
}

void stackOverflow(){
	log_info(logger, "pid %d sufrio stack overflow", pcb_actual->pid);
	char *mensaje = string_new();
	string_append(&mensaje, "Stack Overflow");
	imprimirTexto("Stack Overflow");
	free(mensaje);
	finalizar();

}

char* devolver_ruta_log(){
	int ruta_valida = 0;
	int i = 0;
	char* ruta_log = string_new();
	char* j;
	while(!ruta_valida){
		free(ruta_log);
		ruta_log = string_new();
		string_append(&ruta_log, RUTA_LOG);
		j = string_itoa(i);
		string_append(&ruta_log, j);
		free(j);
		string_append(&ruta_log, ".log");
		if(access( ruta_log, F_OK ) != -1){
			i++;
		} else {
			ruta_valida = 1;
		}
	}
	return ruta_log;
}
