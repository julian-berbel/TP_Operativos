#ifndef CPU_H_
#define CPU_H_

#include "cliente.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include "primitivas.h"
#include <commons/collections/list.h>
#include "pcb.h"
#include "interfazUMC.h"

#define RUTA_LOG "/home/utnso/cpu.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/cpu/config/cpu.config"

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
char* obtener_instruccion(t_PCB * pcb);
char* pedir_instruccion_umc(int num_pagina, int offset, int tamanio);

#endif
