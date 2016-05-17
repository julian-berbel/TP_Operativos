#include<commons/config.h>
#include<commons/log.h>
#include"umc.h"

int main(){
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_t thread_cpu;
	pthread_t thread_nucleo;
	abrirConfiguracion();
	log_info(logger, "Inicia proceso UMC");
	int socket_swap = crear_socket_cliente(ipSwap, puertoSwap);
	log_info(logger_pantalla, "UMC y Swap conectados");
	int socket_servidor = crear_socket_servidor(ipUmc, puertoUmc);
	int socket_cliente = recibirConexion(socket_servidor);
	char* mensaje;
	while(string_is_empty(mensaje = recibir_string_generico(socket_cliente)));
	if (strcmp(mensaje, "n")){
		int *args_nucleo = malloc(sizeof(*args_nucleo));
		if ( args_nucleo == NULL ) {
			log_info(logger_pantalla, "No se pudo asignar memoria para un hilo del CPU");
			cerrar_todo();
		}
		*args_nucleo = socket_cliente;
		int r = pthread_create(&thread_nucleo, NULL, funcion_nucleo, args_nucleo);
		//Hay que hacer el pthread_join en algun lugar del main. Posiblemente en el cerrar_todo.
	}else if (strcmp(mensaje, "c")){
		int *args_cpu = malloc(sizeof(*args_cpu));
		if ( args_cpu == NULL){
			cerrar_todo();
		}
		*args_cpu = socket_cliente;
		pthread_create(&thread_cpu, &attr, funcion_cpu, args_cpu);
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

void abrirConfiguracion(){
	configuracionUMC = config_create(RUTA_CONFIG);
	ipUmc = config_get_string_value(configuracionUMC, "IP_UMC");
	puertoUmc = config_get_string_value(configuracionUMC, "PUERTO");
	ipSwap = config_get_string_value(configuracionUMC, "IP_SWAP");
	puertoSwap = config_get_string_value(configuracionUMC, "PUERTO_SWAP");
	marcos = config_get_int_value(configuracionUMC, "MARCOS");
	marco_size = config_get_int_value(configuracionUMC, "MARCO_SIZE");
	marco_x_proc = config_get_int_value(configuracionUMC, "MARCO_X_PROC");
	entradas_tlb = config_get_int_value(configuracionUMC, "ENTRADAS_TLB");
	retardo = config_get_int_value(configuracionUMC, "RETARDO");
	logger = log_create(RUTA_LOG, "UMC", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "UMC", true, LOG_LEVEL_INFO);

	printf("%s\n", ipUmc);
	printf("%s\n", puertoUmc);
	printf("%s\n", ipSwap);
	printf("%s\n", puertoSwap);
	printf("%d\n", marcos);
	printf("%d\n", marco_size);
	printf("%d\n", marco_x_proc);
	printf("%d\n", entradas_tlb);
	printf("%d\n", retardo);
}

void cerrar_todo(){
	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracionUMC);
}

void *funcion_nucleo(void *argumento){ // thread para nucleo
	int socket_nucleo = *((int *) argumento);
	char* mensaje = string_new();
	string_append(&mensaje, "okey\n");
	enviar_string(socket_nucleo, mensaje);
	free(mensaje);
	free(argumento);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}

void *funcion_cpu(void *argumento){
	int socket_cpu = *((int *) argumento);
	char* mensaje = string_new();
	string_append(&mensaje, "okey\n");
	enviar_string(socket_cpu, mensaje);
	free(mensaje);
	close(socket_cpu);
	free(argumento);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}
