/*
 * interfazNucleo.h
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#ifndef INTERFAZNUCLEO_H_
#define INTERFAZNUCLEO_H_

#include <stdlib.h>

extern int pid;

typedef enum {TERMINAR, IMPRIMIR} interfazPropia;

typedef enum {CANCELAR = 2} interfazNucleo;

int serializarCancelar(void** serializacion);

void deserializarImprimir(void* parametrosSerializados, void* dataAdicional);

void procesarMensaje(void* mensaje, void* dataAdicional);

extern void imprimir(char* mensaje);

extern void terminar();

#endif /* INTERFAZNUCLEO_H_ */
