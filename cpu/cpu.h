#ifndef CPU_H_
#define CPU_H_


#include "cliente.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include "primitivas.h"
#include <commons/collections/list.h>

#define RUTA_LOG "/home/utnso/cpu.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/cpu/config/cpu.config"

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

t_config* configuracionCPU;
char* ipNucleo;
char* puertoNucleo;
char* ipUMC;
char* puertoUMC;
int socket_umc;
t_log* logger;
t_log* logger_pantalla;
int tamanio_pagina;

void abrirConfiguracion();
void cerrar_todo();

#endif
