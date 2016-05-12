#include<commons/config.h>
#include<commons/log.h>
#include"umc.h"

int main(){
	abrirConfiguracion();
	log_info(logger, "Inicia proceso UMC");

	/*int socket_swap = crear_socket_cliente(ipSwap, puertoSwap);

	log_info(logger_pantalla, "UMC y Swap conectados");*/

	int socket_servidor = crear_socket_servidor(ipUmc, puertoUmc);

	int socket_cliente = recibirConexion(socket_servidor);

	char* mensaje;

	while(string_is_empty(mensaje = recibir_string_generico(socket_cliente)));

	//enviar_string(socket_swap, mensaje);
	if (strcmp(mensaje, "n")){
		int args_nucleo = 1234;
		pthread_t thread_nucleo;
		int r = pthread_create(&thread_nucleo, NULL, funcion_nucleo, (void*) args_nucleo);
		printf("Recibi un mensaje de Kernel\n");


	}else if (strcmp(mensaje, "c")){
		printf("Recibir un mensaje de CPU\n");
	}




	free(mensaje);
	//close(socket_swap);
	close(socket_servidor);
	close(socket_cliente);
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

void* funcion_nucleo(void *argumento){ // thread para nucleo
	int socket_nucleo;
	socket_nucleo = (int) argumento;
	char* mensaje = string_new();
	string_append(&mensaje, "okey\n");
	enviar_string(socket_nucleo, mensaje);
	free(mensaje);

}


