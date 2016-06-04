/*
 * interfazUMC.h
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */

#ifndef INTERFAZSWAP_H_
#define INTERFAZSWAP_H_

#include <stdlib.h>

typedef enum { INICIALIZAR, SOLICITAR, ALMACENAR, FINALIZAR} tipoDeMensaje;

void deserializarInicializar(void* parametrosSerializados);

void deserializarSolicitar(void* parametrosSerializados);

void deserializarAlmacenar(void* parametrosSerializados);

void deserializarFinalizar(void* parametrosSerializados);

void procesarMensaje(void* mensaje);

extern void inicializar(int id_programa,int paginas_requeridas, char* programa);
extern void finalizar(int num_programa);
extern void leer_pagina(int num_pagina, int offset,size_t t);
extern void escribir_pagina(int num_pagina, int offset, size_t t, char *buffer);

#endif /* INTERFAZSWAP_H_ */
