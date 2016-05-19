#ifndef NUCLEO_H_
#define NUCLEO_H_

#include "servidor.h"
#include "cliente.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#define RUTA_LOG "/home/utnso/nucleo.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/nucleo/config/nucleo.config"

t_config* configuracionNucleo;
char* ipNucleo;
char* puertoNucleo;
char* puertoCPU;
int quantum;
int quantum_sleep;
char** io_id;
char** io_sleep;
char** sem_id;
char** sem_init;
char** shared_vars;
t_log* logger;
t_log* logger_pantalla;
t_queue colaPCBNew; // hace falta esta?
t_queue colaPCBReady;
t_queue colaPCBExec;
t_queue colaPCBBlock;
t_queue colaPCBExit; // ^

typedef struct{
	int pagina;
	int offset;
	int size;
} t_posicion_de_memoria;

typedef struct{
	char identificador;
	t_posicion_de_memoria posicion;
} t_variable;

typedef struct{
	t_posicion_de_memoria* argumentos;
	t_variable* variables;
	int direccionDeRetorno;
	t_posicion_de_memoria posicionDeRetorno;
} t_elemento_stack;

typedef struct{
	int pid;
	int programCounter;
	int cantidadPaginas;
	int** indiceCodigo;
	char* indiceEtiquetas;
	t_list indiceStack;
} t_PCB;

void abrirConfiguracion();
void cerrar_todo();

#endif
