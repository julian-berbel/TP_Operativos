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

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable){
	return CONTENIDO_VARIABLE;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor){
	return CONTENIDO_VARIABLE;
}

void irAlLabel(t_nombre_etiqueta etiqueta){

}

void llamarSinRetorno(t_nombre_etiqueta etiqueta){

}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar){

}

void finalizar(){

}

void retornar(t_valor_variable retorno){

}

void imprimir(t_valor_variable valor) {
	printf("Imprimir %d\n", valor);
}

void imprimirTexto(char* texto) {
	printf("ImprimirTexto: %s", texto);
}

void wait(t_nombre_semaforo identificador_semaforo){

}

void signal(t_nombre_semaforo identificador_semaforo){

}
