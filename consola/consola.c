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
		procesarMensaje(mensaje, NULL);
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
	char* buffer = NULL;
	long largo;
	FILE* f = fopen (ruta, "rb");

	if (archivoExiste(f)){
		fseek (f, 0, SEEK_END);
		largo = ftell (f);
		fseek (f, 0, SEEK_SET);
		buffer = malloc (largo);
		if (buffer) fread (buffer, 1, largo, f);
		fclose (f);
	}else{
		printf("El archivo no existe!\n");
		exit(1);
	}

	return buffer;
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
