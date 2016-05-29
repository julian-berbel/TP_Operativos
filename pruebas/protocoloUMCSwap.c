/*
 * pruebas2.c
 *
 *  Created on: 29/5/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>

//Comun a UMC y Swap
typedef enum {INICIALIZAR, PLACEHOLDER, LEER_PAGINA} interfazUMCSwap; //placeholder porque en umc.c inicializar -> 1, leer_pagina -> 3. No se que seria el 2.

//Unico a Swap
void inicializar(void* parametrosSerializados){
	int id_programa, paginas_requeridas;
	char* programa;

	memcpy(&id_programa, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	memcpy(&paginas_requeridas, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	programa = parametrosSerializados;

	printf("inicializar id_programa: %d, paginas_requeridas: %d, programa: %s\n", id_programa, paginas_requeridas, programa);
}

void placeholder(void* parametrosSerializados){

}

void leerPagina(void* parametrosSerializados){
	int num_pagina, offset;
	size_t t;

	memcpy(&num_pagina, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	memcpy(&offset, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	memcpy(&t, parametrosSerializados, sizeof(size_t));

	printf("leer pagina num_pagina: %d, offset: %d, t: %d\n", num_pagina, offset, t);
}

void (*funciones[3])(void*) = {inicializar, placeholder, leerPagina};

void procesarMensaje(void* mensaje){
	interfazUMCSwap tipo = *((interfazUMCSwap*) mensaje);
	mensaje += sizeof(interfazUMCSwap);

	(*funciones[tipo])(mensaje);
}

//Unico a UMC
void* serializarInicializar(int id_programa, int paginas_requeridas, char* programa) {
	int tamanio = sizeof(interfazUMCSwap) + sizeof(int) * 2 + sizeof(char) * (string_length(programa) + 1);
	void* mensaje = malloc(tamanio);
	void* aux = mensaje;
	memset(mensaje, 0, tamanio);

	*((interfazUMCSwap*) aux) = INICIALIZAR;
	aux += sizeof(interfazUMCSwap);
	*((int*) aux) = id_programa;
	aux += sizeof(int);
	*((int*) aux) = paginas_requeridas;
	aux += sizeof(int);
	memcpy(aux, programa, sizeof(char) * string_length(programa));

	return mensaje;
}

void* serializarLeerPagina(int num_pagina, int offset, size_t t) {
	void* mensaje = malloc(sizeof(interfazUMCSwap) + sizeof(int) * 3 + sizeof(size_t));
	void* aux = mensaje;

	*((interfazUMCSwap*) aux) = LEER_PAGINA;
	aux += sizeof(interfazUMCSwap);
	*((int*) aux) = num_pagina;
	aux += sizeof(int);
	*((int*) aux) = offset;
	aux += sizeof(int);
	*((int*) aux) = t;

	return mensaje;
}

//Prueba
int main(){
	void* mensaje;

	mensaje = serializarInicializar(3141592, 2718281, "asdasdwqewqego");
	procesarMensaje(mensaje);

	free(mensaje);

	mensaje = serializarLeerPagina(3, 2, 1);
	procesarMensaje(mensaje);

	free(mensaje);

	return 0;
}

