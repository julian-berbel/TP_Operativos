/*
 * pcb.h
 *
 *  Created on: 27/5/2016
 *      Author: utnso
 */

#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>

typedef struct{
	int pagina;
	int offset;
	int size;
} __attribute__((packed)) pos_mem;

typedef struct{
	char nombre_var;
	pos_mem* dir_var;
} t_variable;

typedef struct{
	t_list* args;
	t_list* vars;
	int dir_retorno;
	pos_mem* var_retorno;
} nodo_stack;

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

int** crearIndiceCodigo(t_size cantidadInstrucciones);

void cargarIndiceCodigo(int** indiceCodigo, t_metadata_program* metadata);

int calcularTamanioPCB(t_PCB* pcb);

int serializarPCB(t_PCB* pcb, void** pcbSerializado);

t_PCB* deserializarPCB(void* pcb_serializado);

t_PCB* crearPCB(const char* programa, int pid);

void imprimirPCB(t_PCB* pcb);

void pcb_destroy(t_PCB* unPCB);

#endif /* PCB_H_ */
