/*
 * pcb.h
 *
 *  Created on: 27/5/2016
 *      Author: utnso
 */

#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <parser/parser.h>
#include <commons/collections/list.h>

typedef struct {
	int pagina;
	int offset;
	int size;
} __attribute__((packed)) t_posicion_de_memoria;

typedef struct{
	char identificador;
	t_posicion_de_memoria posicion;
} __attribute__((packed)) t_variable;

typedef struct{
	int cantidadArgumentos;
	t_posicion_de_memoria* argumentos;
	int cantidadVariables;
	t_variable* variables;
	int direccionDeRetorno;
	t_posicion_de_memoria posicionDeRetorno;
} t_elemento_stack;

typedef struct{
	int pid;
	int programCounter;
	int cantidadPaginas;
	int cantidadInstrucciones;
	int** indiceCodigo;
	int tamanioIndiceEtiquetas;
	char* indiceEtiquetas;
	t_list* indiceStack;
} t_PCB;

int calcularTamanioPCB(t_PCB* pcb);

int serializarPCB(t_PCB* pcb, void** pcbSerializado);

t_PCB* deserializarPCB(void* pcb_serializado);

void imprimirPCB(t_PCB* pcb);

#endif /* PCB_H_ */
