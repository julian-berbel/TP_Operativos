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

typedef enum {IMPRIMIR, TERMINAR} interfazPropia;

void deserializarImprimir(void* parametrosSerializados, void* dataAdicional);

void (*deserializadores[2])(void*, void*);

void procesarMensaje(void* mensaje, void* dataAdicional);

extern void imprimir(char* mensaje);

extern void terminar();

#endif /* INTERFAZNUCLEO_H_ */
