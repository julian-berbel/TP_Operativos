/*
 * pruebas.c
 *
 *  Created on: 13/5/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>

//Comun a Nucleo, UMC, y CPU
typedef enum { INICIALIZAR, SOLICITAR, ALMACENAR, FINALIZAR} tipoDeMensaje;

//Unico a UMC
void inicializar(void* parametrosSerializados){
	int pid, paginasRequeridas;
	char* programa;

	memcpy(&pid, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	memcpy(&paginasRequeridas, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	programa = parametrosSerializados;

	printf("inicializar pid: %d, paginas requeridas: %d, programa: %s\n", pid, paginasRequeridas, programa);
}

void solicitar(void* parametrosSerializados){
	int numeroDePagina, offset, tamanio;

	memcpy(&numeroDePagina, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	memcpy(&offset, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	memcpy(&tamanio, parametrosSerializados, sizeof(int));

	printf("solicitar pagina: %d, offset: %d, tamanio: %d\n", numeroDePagina, offset, tamanio);
}

void almacenar(void* parametrosSerializados){
	int numeroDePagina, offset, tamanio;
	char* buffer;

	memcpy(&numeroDePagina, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	memcpy(&offset, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	memcpy(&tamanio, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	buffer = parametrosSerializados;

	printf("almacenar pagina: %d, offset: %d, tamanio: %d, buffer: \"%s\"\n", numeroDePagina, offset, tamanio, buffer);
}

void finalizar(void* parametrosSerializados){
	int pid;
	memcpy(&pid, parametrosSerializados, sizeof(int));

	printf("finalizar pid: %d\n", pid);
}

void (*funciones[4])(void*) = {inicializar, solicitar, almacenar, finalizar};

void procesarMensaje(void* mensaje){
	tipoDeMensaje tipo = *((tipoDeMensaje*) mensaje);
	mensaje += sizeof(tipoDeMensaje);

	(*funciones[tipo])(mensaje);
}

//Unico a Nucleo
void* serializarInicializar(int pid, int paginasRequeridas, char* programa){
	int tamanio = sizeof(tipoDeMensaje) + sizeof(int) * 2 + sizeof(char) * (string_length(programa) + 1);
	void* mensaje = malloc(tamanio);
	void* aux = mensaje;

	memset(mensaje, 0, tamanio);

	*((tipoDeMensaje*) aux) = INICIALIZAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = pid;
	aux += sizeof(int);
	*((int*) aux) = paginasRequeridas;
	aux += sizeof(int);
	memcpy(aux, programa, sizeof(char) * string_length(programa));

	return mensaje;
}

//Unico a CPU? tal vez para nucleo tambien?
void* serializarSolicitar(int numeroDePagina, int offset, int tamanio){
	void* mensaje = malloc(sizeof(tipoDeMensaje) + sizeof(int) * 3);
	void* aux = mensaje;

	*((tipoDeMensaje*) aux) = SOLICITAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = numeroDePagina;
	aux += sizeof(int);
	*((int*) aux) = offset;
	aux += sizeof(int);
	*((int*) aux) = tamanio;

	return mensaje;
}

void* serializarAlmacenar(int numeroDePagina, int offset, int tamanio, char* programa){
	int tamanioMensaje = sizeof(tipoDeMensaje) + sizeof(int) * 3 + sizeof(char) * (string_length(programa) + 1);
	void* mensaje = malloc(tamanioMensaje);
	void* aux = mensaje;

	memset(mensaje, 0, tamanioMensaje);

	*((tipoDeMensaje*) aux) = ALMACENAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = numeroDePagina;
	aux += sizeof(int);
	*((int*) aux) = offset;
	aux += sizeof(int);
	*((int*) aux) = tamanio;
	aux += sizeof(int);
	memcpy(aux, programa, sizeof(char) * string_length(programa));

	return mensaje;
}

//Unico a Nucleo
void* serializarFinalizar(int pid){
	void* mensaje = malloc(sizeof(tipoDeMensaje) + sizeof(int));
	void* aux = mensaje;

	*((tipoDeMensaje*) aux) = FINALIZAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = pid;
	procesarMensaje(mensaje);

	return mensaje;
}

//Prueba
int main(int cantidadArgumentos, char* argumentos[]){

	void* mensaje = serializarInicializar(123, 666, "pa k kieres saber eso jaja slds");

	procesarMensaje(mensaje);

	free(mensaje);

	mensaje = serializarSolicitar(1, 2, 1337);

	procesarMensaje(mensaje);

	free(mensaje);

	mensaje = serializarAlmacenar(456, 789, 1337, "lorem ipsum carpe diem cuam minimum credula postero");

	procesarMensaje(mensaje);

	free(mensaje);

	mensaje = serializarFinalizar(80085);

	free(mensaje);

	return 0;
}

