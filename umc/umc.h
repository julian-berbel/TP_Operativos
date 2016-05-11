#ifndef UMC_H_
#define UMC_H_

#include<stdio.h>
#include<commons/log.h>
#include<commons/config.h>
#include"servidor.h"
#include <pthread.h>
#include"cliente.h"

#define RUTA_LOG "/home/utnso/umc.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/umc/config/umc.config"

t_config* configuracionUMC;
char* puertoUmc;
char* ipUmc;
char* ipSwap;
char* puertoSwap;
int marcos;
int marco_size;
int marco_x_proc;
int entradas_tlb;
int retardo;
t_log* logger;
t_log* logger_pantalla;


void abrirConfiguracion();
void cerrar_todo();
void *funcion_nucleo();

#endif
