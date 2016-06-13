/*
 * interfazUMC.h
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */

#ifndef INTERFAZSWAP_H_
#define INTERFAZSWAP_H_

#include <stdlib.h>

typedef enum {/*INICIALIZAR, TERMINAR,*/ SOLICITAR = 2, ALMACENAR = 3/*, FINALIZAR*/} interfazPropia;

void deserializarInicializar(void* parametrosSerializados, void* dataAdicional);

void deserializarSolicitar(void* parametrosSerializados, void* dataAdicional);

void deserializarAlmacenar(void* parametrosSerializados, void* dataAdicional);

void deserializarFinalizar(void* parametrosSerializados, void* dataAdicional);

void (*deserializadores[5])(void*, void*);

void procesarMensaje(void* mensaje, void* dataAdicional);

extern void terminar();

extern void inicializar(int id_programa,int paginas_requeridas, char* programa);
extern void finalizar(int num_programa);
extern void leer_pagina(int num_pagina, int offset,size_t t);
extern void escribir_pagina(int num_pagina, int offset, size_t t, char *buffer);

#endif /* INTERFAZSWAP_H_ */
