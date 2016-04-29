#ifndef CPU_H_
#define CPU_H_

#include "cliente.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>

#define RUTA_LOG "/home/utnso/cpu.log"
#define RUTA_CONFIG "/home/utnso/workspace/tp-2016-1c-Hellfish-Group/cpu/config/cpu.config"

t_config* configuracionCPU;
char* ipNucleo;
char* puertoNucleo;
char* ipUMC;
char* puertoUMC;
t_log* logger;
t_log* logger_pantalla;

void abrirConfiguracion();
void cerrar_todo();

#endif
