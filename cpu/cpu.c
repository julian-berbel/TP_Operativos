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
	char tipo;
	nodo_stack *nodo;
	pos_mem *posicion_memoria;
	t_variable *var;
	pos_mem* nueva_posicion_memoria;
	t_variable *nueva_variable;
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
				if(nueva_posicion_memoria >= pcb_actual->cantidadPaginas){
					stackOverflow();
				} else {
					list_add(nodo->args, nueva_posicion_memoria);
				}
			} else {
				nueva_posicion_memoria->pagina = posicion_memoria->pagina;
				nueva_posicion_memoria->offset = (posicion_memoria->offset + posicion_memoria->size);
				nueva_posicion_memoria->size = 4;
				if(nueva_posicion_memoria >= pcb_actual->cantidadPaginas){
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
				if(nueva_posicion_memoria >= pcb_actual->cantidadPaginas){
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
				if(nueva_posicion_memoria >= pcb_actual->cantidadPaginas){
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
				if(nueva_posicion_memoria >= pcb_actual->cantidadPaginas){
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
				if(nueva_posicion_memoria >= pcb_actual->cantidadPaginas){
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
	char *ptr;
	int valor_variable = strtol(valor_variable_char, &ptr, 10);
	free(valor_variable_char);
	return valor_variable;
}

void asignar(t_puntero puntero, t_valor_variable variable) {
	int num_pagina = puntero / tamanio_pagina;
	int offset = puntero - (num_pagina * tamanio_pagina);
	char *valor_variable = string_itoa(variable);
	enviar_bytes_umc(num_pagina, offset, 4, valor_variable);
	free(valor_variable);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){
	void* variable_serializada;
	int tamanioMensaje = serializarObtenerValor(variable, &variable_serializada);
	enviar(socket_nucleo, variable_serializada, tamanioMensaje);
	free(variable_serializada);
	char *valor_variable_char = recibir_string_generico(socket_nucleo);
	char *ptr;
	int valor_variable = strtol(valor_variable_char, &ptr, 10);
	free(valor_variable_char);
	return valor_variable;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	void* variable_serializada;
	int tamanioMensaje = serializarGrabarValor(variable, valor, &variable_serializada);
	enviar(socket_nucleo, variable_serializada, tamanioMensaje);
	free(variable_serializada);
	return valor;
}

void irAlLabel(t_nombre_etiqueta etiqueta){
	int program_counter = metadata_buscar_etiqueta(etiqueta, pcb_actual->indiceEtiquetas, pcb_actual->tamanioIndiceEtiquetas);
	if(program_counter == -1){
		printf("No se encontro la etiqueta %s en el indice de etiquetas\n", etiqueta);
	} else {
		pcb_actual->programCounter = program_counter;
	}
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
	int program_counter = metadata_buscar_etiqueta(etiqueta, pcb_actual->indiceEtiquetas, pcb_actual->tamanioIndiceEtiquetas);
	if(program_counter == -1){
		printf("No se encontro la funcion %s en el indice de etiquetas\n", etiqueta);
	} else {
		pcb_actual->programCounter = program_counter;
	}
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
	void* wait_serializado;
	int tamanioMensaje = serializarWait(identificador_semaforo, &wait_serializado);
	enviar(socket_nucleo, wait_serializado, tamanioMensaje);
	free(wait_serializado);
	char* mensaje = recibir_string_generico(socket_nucleo);
	if(strcmp(mensaje, "dale para adelante!") != 0){
		pcb_bloqueado = 1;
	}
}

void signal_primitiva(t_nombre_semaforo identificador_semaforo){
	void* signal_serializado;
	int tamanioMensaje = serializarSignal(identificador_semaforo, &signal_serializado);
	enviar(socket_nucleo, signal_serializado, tamanioMensaje);
	free(signal_serializado);

}

void cerrarCPU(int n){
	if(n == SIGUSR1){
		void* mensaje;
		int tamanioMensaje = serializarCerrarCPU(&mensaje);
		enviar(socket_nucleo, mensaje, tamanioMensaje);
		free(mensaje);
	}
}

int main(){
	abrirConfiguracion();
	signal(SIGUSR1, cerrarCPU);
	log_info(logger, "Inicia proceso CPU");
	socket_nucleo = crear_socket_cliente(ipNucleo, puertoNucleo);
	log_info(logger_pantalla, "CPU y Nucleo conectados");
	socket_umc = crear_socket_cliente(ipUMC, puertoUMC);
	log_info(logger_pantalla, "CPU y UMC conectados");
	void* respuesta = recibir(socket_umc);
	tamanio_pagina = *(int*)respuesta;
	free(respuesta);
	void* mensaje;

	while(!flagTerminar){
		mensaje = recibir(socket_nucleo);
		if(!mensaje)break;
		procesarMensaje(mensaje, NULL);
	}

	close(socket_nucleo);

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
	char* instruccion = string_new();
	void* pedido;
	int tamanioSerializacion = serializarSolicitar(num_pagina, offset, tamanio, &pedido);
	enviar(socket_umc, pedido, tamanioSerializacion);
	char* recibido = recibir(socket_umc);
	string_append(&instruccion, recibido);
	free(recibido);
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
	flagTerminar = 1;
}

void continuarEjecucion(int quantum){
	quantum_definido = quantum;
	ciclosDeQuantum();
}

void desalojar(){
	void* mensaje;
	int tamanioMensaje = serializarPCB(pcb_actual, &mensaje);
	enviar(socket_nucleo, mensaje, tamanioMensaje);
	free(mensaje);
	pcb_destroy(pcb_actual);
}

void stackOverflow(){
	char *mensaje = string_new();
	string_append(&mensaje, "Stack Overflow");
	imprimirTexto(mensaje);
	free(mensaje);
	finalizar();
}
