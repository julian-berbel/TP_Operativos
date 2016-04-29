#include <commons/config.h>
#include<commons/log.h>
#include"nucleo.h"

int main(){
	abrirConfiguracion();
	log_info(logger, "Inicia proceso Núcleo");

	int socket_servidor = crear_socket_servidor(ipNucleo, puertoNucleo);
	int socket_consola = recibirConexion(socket_servidor);

	log_info(logger_pantalla, "Nucleo y Consola conectados");

	int otro_socket_servidor = crear_socket_servidor(ipNucleo, puertoCPU);
	int socket_cpu = recibirConexion(otro_socket_servidor);

	log_info(logger_pantalla, "Nucleo y CPU conectados");

	char* mensaje;

	while(string_is_empty(mensaje = recibir_string_generico(socket_consola)));

	char * mensaje_logger = string_new();
		string_append(&mensaje_logger, "Pase por el Nucleo - ");
		string_append(&mensaje_logger, mensaje);
		log_info(logger_pantalla, mensaje_logger);
		free(mensaje_logger);

	enviar_string(socket_cpu, mensaje);

	free(mensaje);
	close(socket_consola);
	close(socket_servidor);
	close(socket_cpu);
	cerrar_todo();

	return 0;
}



void abrirConfiguracion(){
	configuracionNucleo = config_create(RUTA_CONFIG);
	ipNucleo = config_get_string_value(configuracionNucleo, "IP_NUCLEO");
	puertoNucleo = config_get_string_value(configuracionNucleo, "PUERTO_PROG");
	puertoCPU = config_get_string_value(configuracionNucleo, "PUERTO_CPU");
	quantum = config_get_int_value(configuracionNucleo, "QUANTUM");
	quantum_sleep = config_get_int_value(configuracionNucleo, "QUANTUM_SLEEP");
	io_id = config_get_array_value(configuracionNucleo, "IO_ID");
	io_sleep = config_get_array_value(configuracionNucleo, "IO_SLEEP");
	sem_id = config_get_array_value(configuracionNucleo, "SEM_ID");
	sem_init = config_get_array_value(configuracionNucleo, "SEM_INIT");
	shared_vars = config_get_array_value(configuracionNucleo, "SHARED_VARS");
	logger = log_create(RUTA_LOG, "Nucleo", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "Nucleo", true, LOG_LEVEL_INFO);

	printf("%s\n", ipNucleo);
	printf("%s\n", puertoNucleo);
	printf("%s\n", puertoCPU);
	printf("%d\n", quantum);
	printf("%d\n", quantum_sleep);
	printf("%s\n", io_id[0]);

}

void cerrar_todo(){
	int i;
	int largo_array = sizeof(io_id);
	for(i=0; i<largo_array; ++i)
		free(io_id[i]);
	free(io_id);
	largo_array = sizeof(io_sleep);
	for(i=0; i<largo_array; ++i)
		free(io_sleep[i]);
	free(io_sleep);
	largo_array = sizeof(sem_id);
	for(i=0; i<largo_array; ++i)
		free(sem_id[i]);
	free(sem_id);
	largo_array = sizeof(sem_init);
	for(i=0; i<largo_array; ++i)
		free(sem_init[i]);
	free(sem_init);
	largo_array = sizeof(shared_vars);
	for(i=0; i<largo_array; ++i)
		free(shared_vars[i]);
	free(shared_vars);
	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracionNucleo);
}
