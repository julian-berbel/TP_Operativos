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
#include <commons/collections/list.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include "interfazCPUConsola.h"
#include <sys/epoll.h>
#include <math.h>
#include <sys/inotify.h>
#include <termios.h>
#include <unistd.h>

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
char** sem_init1;
char** shared_vars;
t_log* logger;
t_log* logger_pantalla;
t_queue* colaPCBNew;
t_queue* colaPCBReady;
t_queue* colaPCBExec;
t_queue* colaPCBBlock;
t_queue* colaPCBExit;
t_list* consolas; // lista de sockets de consolas
t_list* cpus; // lista de t_cpus
int socket_umc;
int socket_consolas;
int socket_cpus;
pthread_t hiloConsolas;
pthread_t hiloReceptorCPUs;
sem_t semTerminar;
sig_atomic_t flagTerminar = 0;
int tamanioDePagina;
int descriptor_inotify;

typedef struct{
	pthread_t* hiloCPU;
	int socketCPU;
	int pid;
}t_cpu;

typedef struct{
	int socketConsola;
	int pid;
}t_consola;

void abrirConfiguracion();
void cerrar_todo();

void cancelar(void* consola);

void imprimir(int pid, char* mensaje);

void quantumTerminado(t_PCB* pcbActualizado);

#endif
