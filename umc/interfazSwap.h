/*
 * interfazUMC.h
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */

#ifndef INTERFAZSWAP_H_
#define INTERFAZSWAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include <commons/log.h>

typedef enum { INICIALIZAR, TERMINAR, LEER_PAGINA, ESCRIBIR_PAGINA, FINALIZAR} interfazSwap;

int serializarTerminar(void** serializacion);

int serializarInicializar(int id_programa, int paginas_requeridas, char* programa, void** serializacion);

int serializarLeerPagina(int id_programa, int num_pagina, void** serializacion);

int serializarEscribirPagina(int id_programa, int num_pagina, char* buffer, void** serializacion);

int serializarFinalizar(int id_programa, void** serializacion);

extern t_log* logger;

#endif /* INTERFAZSWAP_H_ */
