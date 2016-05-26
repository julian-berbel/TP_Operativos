/*
 * pruebas.c
 *
 *  Created on: 13/5/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>

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

	return sizeof(int) * 6 + sizeof(int) * pcb->cantidadInstrucciones * 2 + sizeof(char) * string_length(pcb->indiceEtiquetas) + tamanioStack;;
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
	memcpy(posicion, pcb->indiceEtiquetas, sizeof(char) * string_length(pcb->indiceEtiquetas));

	t_elemento_stack* elemento;

	int f;

	*((int*)posicion) = pcb->indiceStack->elements_count;
	posicion += sizeof(int);

	for(i = 0; i < pcb->indiceStack->elements_count;i++){
		elemento = (t_elemento_stack*) list_get(pcb->indiceStack, i);
		memcpy(posicion, &elemento->cantidadArgumentos, sizeof(int));
		posicion += sizeof(int);

		for(f = 0; f < elemento->cantidadArgumentos; i++){
			memcpy(posicion, (elemento->argumentos + f), sizeof(t_posicion_de_memoria));
			posicion += sizeof(t_posicion_de_memoria);
		}

		memcpy(posicion, &elemento->cantidadVariables, sizeof(int));
		posicion += sizeof(int);

		for(f = 0; f < elemento->cantidadVariables; i++){
			memcpy(posicion, (elemento->variables + f), sizeof(t_variable));
			posicion += sizeof(t_variable);
		}

		memcpy(posicion, &elemento->direccionDeRetorno, sizeof(t_posicion_de_memoria));
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

	t_elemento_stack* elemento;

	int f;

	int cantidadElementosStack = *((int*) pcb_serializado);
	pcb_serializado += sizeof(int);

	for(i = 0; i < cantidadElementosStack;i++){
		elemento = malloc(sizeof(t_elemento_stack));
		memcpy(&elemento->cantidadArgumentos, pcb_serializado, sizeof(int));
		pcb_serializado += sizeof(int);

		elemento->argumentos = malloc(sizeof(t_posicion_de_memoria) * elemento->cantidadArgumentos);

		for(f = 0; f < elemento->cantidadArgumentos; i++){
			memcpy((elemento->argumentos + f), pcb_serializado, sizeof(t_posicion_de_memoria));
			pcb_serializado += sizeof(t_posicion_de_memoria);
		}

		memcpy(&elemento->cantidadVariables, pcb_serializado, sizeof(int));
		pcb_serializado += sizeof(int);

		elemento->variables = malloc(sizeof(t_variable) * elemento->cantidadVariables);

		for(f = 0; f < elemento->cantidadVariables; i++){
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

int archivoExiste(char *ruta) {
    struct stat st;
    return !stat(ruta, &st);
}

char* leerArchivo(char* ruta){
	FILE* archivo;
	if (archivoExiste(ruta)){
		archivo = fopen(ruta,"r");

		char* buffer = string_new();

		char* aux = malloc(100);
		memset(aux, '\0', 100);

		while(!feof(archivo)){
			fgets(aux, 99, archivo);
			string_append(&buffer, aux);
			memset(aux, '\0', 100);
		}

		free(aux);
		fclose(archivo);
		return buffer;

	}else{
		printf("El archivo no existe\n");
		exit(1);
	}
}

t_PCB* crearPCB(const char* programa){
	t_PCB* pcb = malloc(sizeof(t_PCB));
	t_metadata_program* metadata = metadata_desde_literal(programa);

	static int pid = 0;
	pcb->pid = pid;
	pid++;
	pcb->programCounter = 0;
	pcb->cantidadPaginas = 5;
	pcb->cantidadInstrucciones = metadata->instrucciones_size;
	pcb->indiceCodigo = crearIndiceCodigo(metadata->instrucciones_size);
	cargarIndiceCodigo(pcb->indiceCodigo, metadata);
	pcb->tamanioIndiceEtiquetas = metadata->etiquetas_size;
	pcb->indiceEtiquetas = metadata->etiquetas;
	pcb->indiceStack = list_create();
	return pcb;
}

void imprimirPCB(t_PCB* pcb){
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
			printf("Pagina: %d, Offset: %d, Size: %d\n", elemento.argumentos[i].pagina, elemento.argumentos[i].offset, elemento.argumentos[i].size);
		}
		printf("\tCantidad de Variables: %d\n", elemento.cantidadVariables);
		for(f = 0; f < elemento.cantidadVariables; f++){
			printf("\t\tVariable %d: ", f);
			printf("Identificador: %c, Pagina: %d, Offset: %d, Size: %d\n", elemento.variables[i].identificador, elemento.variables[i].posicion.pagina, elemento.variables[i].posicion.offset, elemento.variables[i].posicion.size);
		}
		printf("\tDireccion de Retorno: %d\n", elemento.direccionDeRetorno);
		printf("\tPosicion de Retorno:\n");
		printf("\t\tPagina: %d, Offset: %d, Size: %d\n", elemento.posicionDeRetorno.pagina, elemento.posicionDeRetorno.offset, elemento.posicionDeRetorno.size);
	}
}

void agregarElementoAStack(){
	t_PCB* pcb = malloc(sizeof(t_PCB));

	t_elemento_stack* elemento;

	elemento = malloc(sizeof(t_elemento_stack));
	elemento->cantidadArgumentos = 1;

	elemento->argumentos = malloc(sizeof(t_posicion_de_memoria));

	t_posicion_de_memoria* unArgumento = malloc(sizeof(t_posicion_de_memoria));
	unArgumento->offset = 1;
	unArgumento->pagina = 2;
	unArgumento->size = 3;

	memcpy(elemento->argumentos, unArgumento, sizeof(t_posicion_de_memoria));

	elemento->cantidadVariables = 1;

	printf("%d\n", unArgumento->offset);

	elemento->variables = malloc(sizeof(t_variable));

	t_variable* unaVariable = malloc(sizeof(t_variable));
	unaVariable->identificador = 'a';
	unaVariable->posicion.offset = 4;
	unaVariable->posicion.pagina = 5;
	unaVariable->posicion.size = 6;

	memcpy(elemento->variables, unaVariable, sizeof(t_variable));

	elemento->direccionDeRetorno = 7;

	unArgumento->offset = 8;
	unArgumento->pagina = 9;
	unArgumento->size = 10;

	memcpy(&elemento->posicionDeRetorno, unArgumento, sizeof(t_posicion_de_memoria));

	list_add(pcb->indiceStack, elemento);

}

int main(int cantidadArgumentos, char* argumentos[]){
	char* programa = leerArchivo(argumentos[1]);

	t_PCB* pcb = crearPCB(programa);

//	agregarElementoAStack(pcb);

	imprimirPCB(pcb);

	void* serializado = serializarPCB(pcb);

	t_PCB* otroPCB = deserializarPCB(serializado);

	imprimirPCB(otroPCB);

	free(pcb);
	free(programa);
	return 0;
}
