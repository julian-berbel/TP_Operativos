#include <commons/config.h>
#include <commons/log.h>
#include"cpu.h"
#include <parser/parser.h>

int main(){
	abrirConfiguracion();
	log_info(logger, "Inicia proceso CPU");

	int socket_nucleo = crear_socket_cliente(ipNucleo, puertoNucleo);

	log_info(logger_pantalla, "CPU y Nucleo conectados");

	socket_umc = crear_socket_cliente(ipUMC, puertoUMC);

	log_info(logger_pantalla, "CPU y UMC conectados");

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

//primitivas
static const int CONTENIDO_VARIABLE = 20;
static const int POSICION_MEMORIA = 0x10;

t_puntero definirVariable(t_nombre_variable variable) {
	printf("definir la variable %c\n", variable);
	char *mensaje=string_new();
	string_append(&mensaje,"guarde la variable\n");
	enviar_string(socket_umc, mensaje);
	return POSICION_MEMORIA;
}

t_puntero obtenerPosicionVariable(t_nombre_variable variable) {
	printf("Obtener posicion de %c\n", variable);
	return POSICION_MEMORIA;
}

t_valor_variable dereferenciar(t_puntero puntero) {
	printf("Dereferenciar %d y su valor es: %d\n", puntero, CONTENIDO_VARIABLE);
	return CONTENIDO_VARIABLE;
}

void asignar(t_puntero puntero, t_valor_variable variable) {
	printf("Asignando en %d el valor %d\n", puntero, variable);
}

void imprimir(t_valor_variable valor) {
	printf("Imprimir %d\n", valor);
}

void imprimirTexto(char* texto) {
	printf("ImprimirTexto: %s", texto);
}


