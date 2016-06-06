#include <commons/config.h>
#include <commons/log.h>
#include "cpu.h"

AnSISOP_funciones functions = {
		.AnSISOP_definirVariable		= definirVariable,
		.AnSISOP_obtenerPosicionVariable= obtenerPosicionVariable,
		.AnSISOP_dereferenciar			= dereferenciar,
		.AnSISOP_asignar				= asignar,
		.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
		.AnSISOP_asignarValorCompartida = asignarValorCompartida,
		.AnSISOP_irAlLabel              = irAlLabel,
		.AnSISOP_llamarSinRetorno       = llamarSinRetorno,
		.AnSISOP_llamarConRetorno       = llamarConRetorno,
		.AnSISOP_finalizar              = finalizar,
		.AnSISOP_retornar               = retornar,
		.AnSISOP_imprimir				= imprimir,
		.AnSISOP_imprimirTexto			= imprimirTexto,

};
AnSISOP_kernel kernel_functions = {
		.AnSISOP_wait                   = wait,
		.AnSISOP_signal                 = signal,
};

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
	char* instruccion = pedir_instruccion_umc(num_pagina, offset, bytes_tamanio_instruccion);
	return instruccion;
	//Una vez que se usa la instruccion hay que hacer free
}

char* pedir_instruccion_umc(int num_pagina, int offset, int tamanio){
	char* instruccion;
	void* pedido;
	int tamanioSerializacion = serializarSolicitar(num_pagina, offset, tamanio, &pedido);
	enviar(socket_umc, pedido, tamanioSerializacion);
	instruccion = recibir(socket_umc);
	string_append(&instruccion, "Aca va la instruccion que recibo de la UMC\n"); //?
	return instruccion;
}
