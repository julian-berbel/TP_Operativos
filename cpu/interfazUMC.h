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

typedef enum {SOLICITAR = 2, ALMACENAR = 3, CAMBIAR_PROCESO_ACTIVO = 5} interfazUMC;

int serializarSolicitar(int num_pagina, int offset, size_t t, void** serializacion);

int serializarAlmacenar(int num_pagina, int offset, size_t t, char* buffer, void** serializacion);

int serializarCambioDeProcesoActivo(int pid, void** serializacion);

#endif /* INTERFAZUMC_H_ */
