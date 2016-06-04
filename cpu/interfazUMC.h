/*
 * interfazUMC.h
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */

#ifndef INTERFAZUMC_H_
#define INTERFAZUMC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>

typedef enum {SOLICITAR = 1, ALMACENAR = 2} tipoDeMensaje;

int serializarSolicitar(int num_pagina, int offset, size_t t, void** serializacion);

int serializarAlmacenar(int num_pagina, int offset, size_t t, char* buffer, void** serializacion);

#endif /* INTERFAZUMC_H_ */
