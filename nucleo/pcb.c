/*
 * pcb.c
 *
 *  Created on: 27/5/2016
 *      Author: utnso
 */

#include "pcb.h"

int** crearIndiceCodigo(t_size cantidadInstrucciones){
	int** indiceCodigo = malloc(sizeof(int*) * cantidadInstrucciones);

	int i;
	for(i = 0; i < cantidadInstrucciones; i++) indiceCodigo[i] = malloc(sizeof(int) * 2);

	return indiceCodigo;
}

void cargarIndiceCodigo(int** indiceCodigo, t_metadata_program* metadata){
	int i;
	for(i = 0; i < metadata->instrucciones_size; i++){
		indiceCodigo[i][0] = (metadata->instrucciones_serializado + i)->start;
		indiceCodigo[i][1] = (metadata->instrucciones_serializado + i)->offset;
	}
}

int calcularTamanioPCB(t_PCB* pcb){
	int i;
	int tamanioStack = pcb->indiceStack->elements_count * sizeof(int) * 3;
	t_elemento_stack* elemento;

	for(i = 0; i < pcb->indiceStack->elements_count;i++){
		elemento = (t_elemento_stack*) list_get(pcb->indiceStack, i);
		tamanioStack += elemento->cantidadArgumentos * sizeof(t_posicion_de_memoria) + elemento->cantidadVariables * sizeof(t_variable) + sizeof(t_posicion_de_memoria);
	}

	return sizeof(int) * 6 + sizeof(int) * pcb->cantidadInstrucciones * 2 + sizeof(char) * pcb->tamanioIndiceEtiquetas + tamanioStack;;
}

void* serializarPCB(t_PCB* pcb){

	int tamanioSerializado = calcularTamanioPCB(pcb);

	void* pcbSerializado = malloc(tamanioSerializado);

	void* posicion = pcbSerializado;

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

	t_elemento_stack* elemento;

	int f;

	memcpy(posicion, &pcb->indiceStack->elements_count, sizeof(int));
	posicion += sizeof(int);

	for(i = 0; i < pcb->indiceStack->elements_count;i++){
		elemento = (t_elemento_stack*) list_get(pcb->indiceStack, i);
		memcpy(posicion, &elemento->cantidadArgumentos, sizeof(int));
		posicion += sizeof(int);

		for(f = 0; f < elemento->cantidadArgumentos; f++){
			memcpy(posicion, (elemento->argumentos + f), sizeof(t_posicion_de_memoria));
			posicion += sizeof(t_posicion_de_memoria);
		}

		memcpy(posicion, &elemento->cantidadVariables, sizeof(int));
		posicion += sizeof(int);

		for(f = 0; f < elemento->cantidadVariables; f++){
			memcpy(posicion, (elemento->variables + f), sizeof(t_variable));
			posicion += sizeof(t_variable);
		}

		memcpy(posicion, &elemento->direccionDeRetorno, sizeof(int));
		posicion += sizeof(int);

		memcpy(posicion, &elemento->posicionDeRetorno, sizeof(t_posicion_de_memoria));
		posicion += sizeof(t_posicion_de_memoria);
	}

	return pcbSerializado;
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

	t_elemento_stack* elemento;

	int f;

	int cantidadElementosStack = *((int*) pcb_serializado);
	pcb_serializado += sizeof(int);

	pcb->indiceStack = list_create();

	for(i = 0; i < cantidadElementosStack;i++){
		elemento = malloc(sizeof(t_elemento_stack));
		memcpy(&elemento->cantidadArgumentos, pcb_serializado, sizeof(int));
		pcb_serializado += sizeof(int);

		elemento->argumentos = malloc(sizeof(t_posicion_de_memoria) * elemento->cantidadArgumentos);

		for(f = 0; f < elemento->cantidadArgumentos; f++){
			memcpy((elemento->argumentos + f), pcb_serializado, sizeof(t_posicion_de_memoria));
			pcb_serializado += sizeof(t_posicion_de_memoria);
		}

		memcpy(&elemento->cantidadVariables, pcb_serializado, sizeof(int));
		pcb_serializado += sizeof(int);

		elemento->variables = malloc(sizeof(t_variable) * elemento->cantidadVariables);

		for(f = 0; f < elemento->cantidadVariables; f++){
			memcpy((elemento->variables + f), pcb_serializado, sizeof(t_variable));
			pcb_serializado += sizeof(t_variable);
		}

		memcpy(&elemento->direccionDeRetorno, pcb_serializado, sizeof(int));
		pcb_serializado += sizeof(int);
		memcpy(&elemento->posicionDeRetorno, pcb_serializado, sizeof(t_posicion_de_memoria));
		pcb_serializado += sizeof(t_posicion_de_memoria);
		list_add(pcb->indiceStack, (void*) elemento);
	}

	return pcb;
}

t_PCB* crearPCB(const char* programa){
	static int pid = 0;

	t_PCB* pcb = malloc(sizeof(t_PCB));
	t_metadata_program* metadata = metadata_desde_literal(programa);

	pcb->pid = pid;
	pid++; //sincronizar con mutex?

	pcb->programCounter = 0;
//	pcb->cantidadPaginas = string_length(programa)*sizeof(char) / tamanioDePagina; // tamanioDePagina lo tiene que pasar la umc por socket
	pcb->cantidadInstrucciones = metadata->instrucciones_size;
	pcb->indiceCodigo = crearIndiceCodigo(metadata->instrucciones_size);
	cargarIndiceCodigo(pcb->indiceCodigo, metadata);
	pcb->tamanioIndiceEtiquetas = metadata->etiquetas_size;
	pcb->indiceEtiquetas = metadata->etiquetas;
	pcb->indiceStack = list_create();

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
	t_elemento_stack elemento;
	printf("Cantidad Elementos: %d\n", pcb->indiceStack->elements_count);
	int f;
	for(i = 0; i < pcb->indiceStack->elements_count; i++){
		printf("Elemento %d:\n", i);
		elemento = *((t_elemento_stack*)list_get(pcb->indiceStack, i));
		printf("\tCantidad de Argumentos: %d\n", elemento.cantidadArgumentos);
		for(f = 0; f < elemento.cantidadArgumentos; f++){
			printf("\t\tArgumento %d: ", f);
			printf("Pagina: %d, Offset: %d, Size: %d\n", elemento.argumentos[f].pagina, elemento.argumentos[f].offset, elemento.argumentos[f].size);
		}
		printf("\tCantidad de Variables: %d\n", elemento.cantidadVariables);
		for(f = 0; f < elemento.cantidadVariables; f++){
			printf("\t\tVariable %d: ", f);
			printf("Identificador: %c, Pagina: %d, Offset: %d, Size: %d\n", elemento.variables[f].identificador, elemento.variables[f].posicion.pagina, elemento.variables[f].posicion.offset, elemento.variables[f].posicion.size);
		}
		printf("\tDireccion de Retorno: %d\n", elemento.direccionDeRetorno);
		printf("\tPosicion de Retorno:\n");
		printf("\t\tPagina: %d, Offset: %d, Size: %d\n", elemento.posicionDeRetorno.pagina, elemento.posicionDeRetorno.offset, elemento.posicionDeRetorno.size);
	}
}
