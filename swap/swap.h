#ifndef SWAP_H_
#define SWAP_H_

#include<stdio.h>
#include<commons/log.h>
#include<commons/config.h>
#include"servidor.h"

#define RUTA_LOG "/home/utnso/swap.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/swap/config/swap.config"

typedef struct{
	int pid;
	int pagina;
	int posicion;
	int offset;
	int bit_uso;
} t_swap;


t_config* configuracion_swap;
char* puertoSwap;
char* ipSwap;
char* nombre_data;
int cant_paginas;
int pagina_size;
int retardo_compactacion;
t_log* logger;
t_log* logger_pantalla;

t_swap lista_procesos[1000];

void abrirConfiguracion();
void cerrar_todo();


#endif
