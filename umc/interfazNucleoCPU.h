/*
 * interfazUMC.h
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */

#ifndef INTERFAZNUCLEOCPU_H_
#define INTERFAZNUCLEOCPU_H_

#include <stdlib.h>
#include <commons/log.h>
#include <string.h>
#include <stdio.h>
#include <commons/string.h>

typedef enum {/*INICIALIZAR, TERMINAR,*/ SOLICITAR = 2, ALMACENAR = 3/*, FINALIZAR*/, CAMBIAR_PROCESO_ACTIVO} interfazPropia;

void deserializarInicializar(void* parametrosSerializados, void* dataAdicional);

void deserializarSolicitar(void* parametrosSerializados, void* dataAdicional);

void deserializarAlmacenar(void* parametrosSerializados, void* dataAdicional);

void deserializarFinalizar(void* parametrosSerializados, void* dataAdicional);

void deserializarCambiarProcesoActivo(void* parametrosSerializados, void* dataAdicional);

void (*deserializadores[6])(void*, void*);

void procesarMensaje(void* mensaje, void* dataAdicional);

extern void terminar();

extern void inicializar(int id_programa,int paginas_requeridas, char* programa, void* cliente);
extern void finalizar(int num_programa);
extern void leer_pagina(int num_pagina, int offset,size_t t, void* cliente);
extern void escribir_pagina(int num_pagina, int offset, size_t t, char *buffer,void* cliente);
extern void cambiar_proceso_activo(int pid, void* dataAdicional);

extern t_log* logger;

#endif /* INTERFAZNUCLEOCPU_H_ */
