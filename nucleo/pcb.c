/*
 * pcb.c
 *
 *  Created on: 27/5/2016
 *      Author: utnso
 */

#include "pcb.h"

int** crearIndiceCodigo(t_size cantidadInstrucciones){
	log_info(logger, "Creando Indice de Codigo");
	int** indiceCodigo = malloc(sizeof(int*) * cantidadInstrucciones);

	int i;
	for(i = 0; i < cantidadInstrucciones; i++) indiceCodigo[i] = malloc(sizeof(int) * 2);

	return indiceCodigo;
}

void cargarIndiceCodigo(int** indiceCodigo, t_metadata_program* metadata){
	log_info(logger, "Cargando Indice de Codigo");
	int i;
	for(i = 0; i < metadata->instrucciones_size; i++){
		indiceCodigo[i][0] = (metadata->instrucciones_serializado + i)->start;
		indiceCodigo[i][1] = (metadata->instrucciones_serializado + i)->offset;
	}
}

int calcularTamanioPCB(t_PCB* pcb){
	log_info(logger, "Calculando tamanio de PCB: pid: %d", pcb->pid);
	int tamanioPCB = sizeof(int) * 6 + pcb->cantidadInstrucciones * 2 * sizeof(int) + pcb->tamanioIndiceEtiquetas * sizeof(char);
	nodo_stack* elemento;

	int i;
	for(i = 0; i < pcb->indiceStack->elements_count; i++){
		elemento = list_get(pcb->indiceStack, i);
		tamanioPCB += sizeof(int) * 2 + elemento->args->elements_count * sizeof(pos_mem) + elemento->vars->elements_count * (sizeof(pos_mem) + sizeof(char)) + sizeof(int) + sizeof(pos_mem);
	}

	return tamanioPCB;
}

int serializarPCB(t_PCB* pcb, void** pcbSerializado){
	log_info(logger, "Serializando PCB: pid: %d", pcb->pid);

	int tamanioSerializado = calcularTamanioPCB(pcb);
	if(!*pcbSerializado)*pcbSerializado = malloc(tamanioSerializado);

	void* posicion = *pcbSerializado;

	memcpy(posicion, &pcb->pid, sizeof(int));
	posicion += sizeof(int);
	memcpy(posicion, &pcb->programCounter, sizeof(int));
	posicion += sizeof(int);
	memcpy(posicion, &pcb->cantidadPaginas, sizeof(int));
	posicion += sizeof(int);
	memcpy(posicion, &pcb->cantidadInstrucciones, sizeof(int));
	posicion += sizeof(int);

	int i;

	for(i = 0; i < pcb->cantidadInstrucciones; i++){
		memcpy(posicion, &pcb->indiceCodigo[i][0], sizeof(int));
		posicion += sizeof(int);
		memcpy(posicion, &pcb->indiceCodigo[i][1], sizeof(int));
		posicion += sizeof(int);
	}

	memcpy(posicion, &pcb->tamanioIndiceEtiquetas,sizeof(int));
	posicion += sizeof(int);

	memcpy(posicion, pcb->indiceEtiquetas, sizeof(char) * pcb->tamanioIndiceEtiquetas);
	posicion += sizeof(char) * pcb->tamanioIndiceEtiquetas;

	nodo_stack* elemento;
	t_variable* variable;

	int f;

	memcpy(posicion, &pcb->indiceStack->elements_count, sizeof(int));
	posicion += sizeof(int);

	for(i = 0; i < pcb->indiceStack->elements_count;i++){
		elemento = (nodo_stack*) list_get(pcb->indiceStack, i);
		memcpy(posicion, &elemento->args->elements_count, sizeof(int));
		posicion += sizeof(int);

		for(f = 0; f < elemento->args->elements_count; f++){
			memcpy(posicion, list_get(elemento->args, f), sizeof(pos_mem));
			posicion += sizeof(pos_mem);
		}

		memcpy(posicion, &elemento->vars->elements_count, sizeof(int));
		posicion += sizeof(int);

		for(f = 0; f < elemento->vars->elements_count; f++){
			variable = list_get(elemento->vars, f);
			memcpy(posicion, &variable->nombre_var, sizeof(char));
			posicion += sizeof(char);
			memcpy(posicion, variable->dir_var, sizeof(pos_mem));
			posicion += sizeof(pos_mem);
		}

		memcpy(posicion, &elemento->dir_retorno, sizeof(int));
		posicion += sizeof(int);

		memcpy(posicion, elemento->var_retorno, sizeof(pos_mem));
		posicion += sizeof(pos_mem);
	}

	return tamanioSerializado;
}

t_PCB* deserializarPCB(void* pcb_serializado){
	t_PCB* pcb = malloc(sizeof(t_PCB));

	memcpy(&pcb->pid, pcb_serializado, sizeof(int));
	pcb_serializado += sizeof(int);
	memcpy(&pcb->programCounter, pcb_serializado, sizeof(int));
	pcb_serializado += sizeof(int);
	memcpy(&pcb->cantidadPaginas, pcb_serializado, sizeof(int));
	pcb_serializado += sizeof(int);
	memcpy(&pcb->cantidadInstrucciones, pcb_serializado, sizeof(int));
	pcb_serializado += sizeof(int);

	int i;
	pcb->indiceCodigo = crearIndiceCodigo(pcb->cantidadInstrucciones);

	for(i = 0; i < pcb->cantidadInstrucciones; i++){
		memcpy(&pcb->indiceCodigo[i][0], pcb_serializado, sizeof(int));
		pcb_serializado += sizeof(int);
		memcpy(&pcb->indiceCodigo[i][1], pcb_serializado, sizeof(int));
		pcb_serializado += sizeof(int);
	}

	memcpy(&pcb->tamanioIndiceEtiquetas, pcb_serializado,sizeof(int));
	pcb_serializado += sizeof(int);

	pcb->indiceEtiquetas = malloc(sizeof(char) * pcb->tamanioIndiceEtiquetas);
	memcpy(pcb->indiceEtiquetas, pcb_serializado, sizeof(char) * pcb->tamanioIndiceEtiquetas);
	pcb_serializado += sizeof(char) * pcb->tamanioIndiceEtiquetas;

	nodo_stack* elemento;
	pos_mem* argumento;
	t_variable* variable;

	int f;
	int cantidadArgumentos;
	int cantidadVariables;

	int cantidadElementosStack = *((int*) pcb_serializado);
	pcb_serializado += sizeof(int);

	pcb->indiceStack = list_create();

	for(i = 0; i < cantidadElementosStack;i++){
		elemento = malloc(sizeof(nodo_stack));

		elemento->args = list_create();

		cantidadArgumentos = *((int*)pcb_serializado);
		pcb_serializado += sizeof(int);

		for(f = 0; f < cantidadArgumentos; f++){
			argumento = malloc(sizeof(pos_mem));
			*argumento = *((pos_mem*)pcb_serializado);
			list_add(elemento->args,(void*) argumento);
			pcb_serializado += sizeof(pos_mem);
		}

		cantidadVariables = *((int*)pcb_serializado);
		pcb_serializado += sizeof(int);

		elemento->vars = list_create();

		for(f = 0; f < cantidadVariables; f++){
			variable = malloc(sizeof(t_variable));
			variable->nombre_var = *((char*) pcb_serializado);
			pcb_serializado += sizeof(char);
			variable->dir_var = malloc(sizeof(pos_mem));
			*variable->dir_var = *((pos_mem*) pcb_serializado);
			pcb_serializado += sizeof(pos_mem);
			list_add(elemento->vars, (void*) variable);
		}

		elemento->dir_retorno = *((int*)pcb_serializado);
		pcb_serializado += sizeof(int);
		elemento->var_retorno = malloc(sizeof(pos_mem));
		*elemento->var_retorno = *((pos_mem*)pcb_serializado);
		pcb_serializado += sizeof(pos_mem);
		list_add(pcb->indiceStack, (void*) elemento);
	}

	log_info(logger, "Deserializado PCB: pid: %d", pcb->pid);
	return pcb;
}

t_PCB* crearPCB(const char* programa, int pid){
	log_info(logger, "Creando PCB: pid: %d", pid);
	t_PCB* pcb = malloc(sizeof(t_PCB));
	t_metadata_program* metadata = metadata_desde_literal(programa);

	pcb->pid = pid;

	pcb->programCounter = metadata->instruccion_inicio;
	pcb->cantidadInstrucciones = metadata->instrucciones_size;
	pcb->indiceCodigo = crearIndiceCodigo(metadata->instrucciones_size);
	cargarIndiceCodigo(pcb->indiceCodigo, metadata);
	pcb->tamanioIndiceEtiquetas = metadata->etiquetas_size;
	pcb->indiceEtiquetas = malloc(sizeof(char) * pcb->tamanioIndiceEtiquetas);
	memcpy(pcb->indiceEtiquetas, metadata->etiquetas, sizeof(char) * pcb->tamanioIndiceEtiquetas);
	pcb->indiceStack = list_create();

	metadata_destruir(metadata);
	return pcb;
}

void imprimirPCB(t_PCB* pcb){ // lo dejo por si alguna vez hace falta
	printf("Pid: %d\n", pcb->pid);
	printf("Program Counter: %d\n", pcb->programCounter);
	printf("Cantidad de Paginas: %d\n", pcb->cantidadPaginas);
	printf("Cantidad de Instrucciones: %d\n", pcb->cantidadInstrucciones);

	printf("\n-------Indice de Codigo-------\n");
	int i;
	for(i = 0; i < pcb->cantidadInstrucciones; i++){
		printf("Instruccion %d:  \t%d %d\n", i, pcb->indiceCodigo[i][0], pcb->indiceCodigo[i][1]);
	}

	printf("\n-------Indice de Etiquetas-------\n");
	printf("%s\n", pcb->indiceEtiquetas);

	printf("\n-------Indice de Stack-------\n");
	nodo_stack elemento;
	pos_mem* argumento;
	t_variable* variable;
	printf("Cantidad Elementos: %d\n", pcb->indiceStack->elements_count);
	int f;
	for(i = 0; i < pcb->indiceStack->elements_count; i++){
		printf("Elemento %d:\n", i);
		elemento = *((nodo_stack*)list_get(pcb->indiceStack, i));
		printf("\tCantidad de Argumentos: %d\n", elemento.args->elements_count);
		for(f = 0; f < elemento.args->elements_count; f++){
			printf("\t\tArgumento %d: ", f);
			argumento = list_get(elemento.args, f);
			printf("Pagina: %d, Offset: %d, Size: %d\n", argumento->pagina, argumento->offset, argumento->size);
		}
		printf("\tCantidad de Variables: %d\n", elemento.vars->elements_count);
		for(f = 0; f < elemento.vars->elements_count; f++){
			printf("\t\tVariable %d: ", f);
			variable = list_get(elemento.vars, f);
			printf("Identificador: %c, Pagina: %d, Offset: %d, Size: %d\n", variable->nombre_var, variable->dir_var->pagina, variable->dir_var->offset, variable->dir_var->size);
		}
		printf("\tDireccion de Retorno: %d\n", elemento.dir_retorno);
		printf("\tPosicion de Retorno:\n");
		printf("\t\tPagina: %d, Offset: %d, Size: %d\n", elemento.var_retorno->pagina, elemento.var_retorno->offset, elemento.var_retorno->size);
	}
}

void pcb_destroy(t_PCB* unPCB){
	log_info(logger, "Destruyendo PCB, pid: %d", unPCB->pid);
	int i, f;
	for(i = 0; i < unPCB->cantidadInstrucciones; i++){
		free(unPCB->indiceCodigo[i]);
	}
	free(unPCB->indiceCodigo);

	free(unPCB->indiceEtiquetas);

	nodo_stack* elemento;
	t_variable* variable;

	for(i = 0; i < unPCB->indiceStack->elements_count; i++){
		elemento = list_get(unPCB->indiceStack, i);

		for(f = 0; f < elemento->args->elements_count; f++){
					free(list_get(elemento->args, f));
		}
		list_destroy(elemento->args);

		free(elemento->var_retorno);

		for(f = 0; f < elemento->vars->elements_count; f++){
			variable = list_get(elemento->vars, f);
			free(variable->dir_var);
			free(variable);
		}
		list_destroy(elemento->vars);
		free(elemento);
	}
	list_destroy(unPCB->indiceStack);
	free(unPCB);
}
