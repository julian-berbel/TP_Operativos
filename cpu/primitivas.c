#include "primitivas.h"
#include "cpu.h"



t_puntero definirVariable(t_nombre_variable variable) {
	printf("definir la variable %c\n", variable);
	char *mensaje=string_new();
	string_append(&mensaje,"umc: guarde la variable\n");
	enviar_string(socket_umc, mensaje);
	return POSICION_MEMORIA;
}

t_puntero obtenerPosicionVariable(t_nombre_variable variable) {
	printf("Obtener posicion de %c\n", variable);
	char *mensaje=string_new();
	string_append(&mensaje,"umc: busque la variable\n");
	enviar_string(socket_umc, mensaje);
	return POSICION_MEMORIA;
}

t_valor_variable dereferenciar(t_puntero puntero) {
	printf("Dereferenciar %d y su valor es: %d\n", puntero, CONTENIDO_VARIABLE);
	char *mensaje=string_new();
		string_append(&mensaje,"umc: dereferencie la variable\n");
		enviar_string(socket_umc, mensaje);
	return CONTENIDO_VARIABLE;
}

void asignar(t_puntero puntero, t_valor_variable variable) {
	printf("Asignando en %d el valor %d\n", puntero, variable);
	char *mensaje=string_new();
		string_append(&mensaje,"umc: asigne la variable\n");
		enviar_string(socket_umc, mensaje);
}

void imprimir(t_valor_variable valor) {
	printf("Imprimir %d\n", valor);
}

void imprimirTexto(char* texto) {
	printf("ImprimirTexto: %s", texto);
}
