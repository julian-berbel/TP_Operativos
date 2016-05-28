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

typedef enum { INICIALIZAR, SOLICITAR, ALMACENAR, FINALIZAR} tipoDeMensaje;

void inicializar(void* parametrosSerializados){
	int pid, paginasRequeridas;

	memcpy(&pid, parametrosSerializados, sizeof(int));
	parametrosSerializados += sizeof(int);

	memcpy(&paginasRequeridas, parametrosSerializados, sizeof(int));

	printf("inicializar pid: %d, paginas requeridas: %d\n", pid, paginasRequeridas);
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
	tipoDeMensaje* tipo = malloc(sizeof(tipoDeMensaje));
	memcpy(tipo, mensaje, sizeof(tipoDeMensaje));
	mensaje += sizeof(tipoDeMensaje);

	(*funciones[*tipo])(mensaje);
}

int main(int cantidadArgumentos, char* argumentos[]){
	void* mensaje = malloc(sizeof(tipoDeMensaje) + sizeof(int) * 3);
	void* aux = mensaje;

	*((tipoDeMensaje*) aux) = INICIALIZAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = 123;
	aux += sizeof(int);
	*((int*) aux) = 666;
	procesarMensaje(mensaje);

	aux = mensaje;

	*((tipoDeMensaje*) aux) = SOLICITAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = 1;
	aux += sizeof(int);
	*((int*) aux) = 2;
	aux += sizeof(int);
	*((int*) aux) = 1337;
	procesarMensaje(mensaje);

	char* mensajeAAlmacenar = "lorem ipsum carpe diem cuam minimum credula postero";
	void* mensajeDinamicoParaAlmacenar = malloc(sizeof(tipoDeMensaje) + sizeof(int) * 3 + sizeof(char) * string_length(mensajeAAlmacenar));
	void* auxParaMensajeDinamico = mensajeDinamicoParaAlmacenar;

	*((tipoDeMensaje*) auxParaMensajeDinamico) = ALMACENAR;
	auxParaMensajeDinamico += sizeof(tipoDeMensaje);
	*((int*) auxParaMensajeDinamico) = 456;
	auxParaMensajeDinamico += sizeof(int);
	*((int*) auxParaMensajeDinamico) = 789;
	auxParaMensajeDinamico += sizeof(int);
	*((int*) auxParaMensajeDinamico) = 1337;
	auxParaMensajeDinamico += sizeof(int);
	memcpy(auxParaMensajeDinamico, mensajeAAlmacenar, sizeof(char) * string_length(mensajeAAlmacenar));
	procesarMensaje(mensajeDinamicoParaAlmacenar);

	aux = mensaje;

	*((tipoDeMensaje*) aux) = FINALIZAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = 80085;
	procesarMensaje(mensaje);

	return 0;
}
