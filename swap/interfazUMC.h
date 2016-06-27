/*
 * interfazUMC.h
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */

#ifndef INTERFAZSWAP_H_
#define INTERFAZSWAP_H_

#include <stdlib.h>
#include <commons/log.h>

typedef enum { INICIALIZAR, TERMINAR, LEER_PAGINA, ESCRIBIR_PAGINA, FINALIZAR} interfazPropia;

void deserializarInicializar(void* parametrosSerializados, void* dataAdicional);

void deserializarLeerPagina(void* parametrosSerializados, void* dataAdicional);

void deserializarEscribirPagina(void* parametrosSerializados, void* dataAdicional);

void deserializarFinalizar(void* parametrosSerializados, void* dataAdicional);

void (*deserializadores[5])(void*, void*);

void procesarMensaje(void* mensaje, void* dataAdicional);

extern void terminar();

extern void inicializar(int id_programa,int paginas_requeridas, char* programa);
extern void finalizar(int num_programa);
extern void leer_pagina(int id_programa, int num_pagina);
extern void escribir_pagina(int id_programa, int num_pagina, char* buffer);

extern t_log* logger;

#endif /* INTERFAZSWAP_H_ */
