#include<commons/config.h>
#include<commons/log.h>
#include"consola.h"

int main(){
	abrirConfiguracion();
	log_info(logger, "Inicia proceso Consola");

	int socket_nucleo = crear_socket_cliente(ipNucleo, puertoNucleo);

	log_info(logger_pantalla, "Consola y Nucleo conectados");

	char* comando = malloc(20);
	memset(comando, '\0', 20);
	printf("Introduzca 'prueba' para enviar mensaje\n");
	scanf("%s", comando);
	enviar_string(socket_nucleo, comando);

	close(socket_nucleo);
	free(comando);
	cerrar_todo();

	return 0;
}

void abrirConfiguracion(){
	configuracionConsola = config_create(RUTA_CONFIG);
	ipNucleo = config_get_string_value(configuracionConsola, "IP_NUCLEO");
	puertoNucleo = config_get_string_value(configuracionConsola, "PUERTO_NUCLEO");
	logger = log_create(RUTA_LOG, "Consola", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "Consola", true, LOG_LEVEL_INFO);

	printf("%s\n", ipNucleo);
	printf("%s\n", puertoNucleo);

}

void cerrar_todo(){
	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracionConsola);
}
