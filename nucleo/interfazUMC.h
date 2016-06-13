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

typedef enum {INICIALIZAR, /*TERMINAR,*/ FINALIZAR = 4} interfazUMC;

//int serializarTerminar(void** serializacion); - En interfazCPU.h

int serializarInicializar(int id_programa, int paginas_requeridas, char* programa, void** serializacion);

int serializarFinalizar(int id_programa, void** serializacion);

#endif /* INTERFAZUMC_H_ */
