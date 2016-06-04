#ifndef NUCLEO_H_
#define NUCLEO_H_

#include "servidor.h"
#include "cliente.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include <commons/collections/queue.h>
#include "pcb.h"
#include "interfazUMC.h"

#define RUTA_LOG "/home/utnso/nucleo.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/nucleo/config/nucleo.config"

t_config* configuracionNucleo;
char* ipNucleo;
char* puertoNucleo;
char* puertoCPU;
char* puertoUMC;
char* ipUMC;
int quantum;
int quantum_sleep;
char** io_id;
char** io_sleep;
char** sem_id;
char** sem_init;
char** shared_vars;
t_log* logger;
t_log* logger_pantalla;
t_queue* colaPCBNew;
t_queue* colaPCBReady;
t_queue* colaPCBExec;
t_queue* colaPCBBlock;
t_queue* colaPCBExit;

void abrirConfiguracion();
void cerrar_todo();

#endif
