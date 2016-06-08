#include "consola.h"

int main(int cantidadArgumentos, char* argumentos[]){
	char* programa = leerArchivo(RUTA_PROGRAMA);
	signal(SIGINT, senialTerminar);

	abrirConfiguracion();
	log_info(logger, "Inicia proceso Consola");

	socket_nucleo = crear_socket_cliente(ipNucleo, puertoNucleo);

	log_info(logger_pantalla, "Consola y Nucleo conectados");

	enviar_string(socket_nucleo, programa);

	free(programa);

	void* mensaje;

	while(!flagTerminar){
		mensaje = recibir(socket_nucleo);
		procesarMensaje(mensaje);
	}

	close(socket_nucleo);
	cerrar_todo();

	return 0;
}

void abrirConfiguracion(){
	configuracionConsola = config_create(RUTA_CONFIG);
	ipNucleo = config_get_string_value(configuracionConsola, "IP_NUCLEO");
	puertoNucleo = config_get_string_value(configuracionConsola, "PUERTO_NUCLEO");
	logger = log_create(RUTA_LOG, "Consola", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "Consola", true, LOG_LEVEL_INFO);
}

void cerrar_todo(){
	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracionConsola);
}

int archivoExiste(char *ruta) {
    struct stat st;
    return !stat(ruta, &st);
}

char* leerArchivo(char* ruta){
	FILE* archivo;
	if (archivoExiste(ruta)){
		archivo = fopen(ruta,"r");

		char* buffer = string_new();

		char* aux = malloc(100);
		memset(aux, '\0', 100);

		while(!feof(archivo)){
			fgets(aux, 99, archivo);
			string_append(&buffer, aux);
			memset(aux, '\0', 100);
		}

		free(aux);
		fclose(archivo);
		return buffer;

	}else{
		printf("El archivo no existe\n");
		exit(1);
	}
}

void imprimir(char* mensaje){
	printf("%s\n", mensaje);
}

void senialTerminar(int n){
	if(n == SIGINT){
		void* mensaje;
		int tamanio;
		tamanio = serializarCancelar(&mensaje);

		enviar(socket_nucleo, mensaje, tamanio);
		free(mensaje);
		terminar();
	}
}

void terminar(){
	flagTerminar = 1;
	shutdown(socket_nucleo, 0);
}
