#ifndef NUCLEO_H_
#define NUCLEO_H_

#include "servidor.h"
#include "cliente.h"
#include <commons/config.h>
#include <stdio.h>
#include <commons/collections/queue.h>
#include "pcb.h"
#include "interfazUMC.h"
#include <commons/collections/list.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include "interfazCPUConsola.h"
#include <math.h>
#include <sys/inotify.h>

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
char** sem_inits;
char** shared_vars;
int stack_size;
t_log* logger;
t_log* logger_pantalla;
t_queue* colaPCBReady;
t_queue* colaPCBExec;
t_list* listaPCBBlock;
t_list* listaPCBExit;
t_list* consolas; // lista de sockets de consolas
t_list* cpus; // lista de t_cpus
int socket_umc;
int socket_consolas;
int socket_cpus;
pthread_t hiloConsolas;
pthread_t hiloReceptorCPUs;
pthread_t hiloPlanificador;
sem_t semTerminar;
sig_atomic_t flagTerminar = 0;
int tamanioDePagina;
int descriptor_inotify;
sem_t moverPCBs;

typedef enum{READY, EXEC, BLOCK, EXIT} estadoACambiar;

typedef struct{
	estadoACambiar estado;
	t_PCB* pcb;
}t_elemento_cola;

typedef struct{
	pthread_t* hiloCPU;
	int socketCPU;
	t_elemento_cola* elemento;
}t_cpu;

typedef struct{
	int socketConsola;
	t_elemento_cola* elemento;
}t_consola;

typedef struct{
	pthread_t hiloDispositivo;
	sem_t semaforoDispositivo;
	int retardo;
	t_queue* cola_dispositivo;
}t_dispositivo;

typedef struct{
	int cantidadDeOperaciones;
	t_elemento_cola** elemento;
}t_pedido;

typedef struct{
	int valor;
	t_queue* cola_bloqueados;
}t_semaforo;

t_dispositivo* dispositivos;
t_semaforo* semaforosGlobales;
int* variablesGlobales;

pthread_mutex_t mutexVariablesGlobales = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexColaReady = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexPID = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCPUs = PTHREAD_MUTEX_INITIALIZER;

void abrirConfiguracion();
void cerrar_todo();

void cancelar(void* consola);

void imprimir(char* mensaje, void* cpu);

void quantumTerminado(void* cpu);

int indiceEnArray(char** array, char* elemento);

int tamanioArray(char** array);

void obtener_valor(char* identificador, void* cpu);

void grabar_valor(char* identificador, int valorAGrabar);

void esperar(char* identificador, void* cpu);

void avisar(char* identificador);

void entradaSalida(char* identificador, int operaciones, void* cpu);

void threadDispositivo(t_dispositivo* dispositivo);

void threadPlanificador();

_Bool hayCPUsLibres();

t_cpu* cpuLibre();

void matarProceso(t_elemento_cola* elemento);

void cerrarCPU(void* cpu, void* ultimoMensaje);

void programaTerminado(void* cpu);

#endif
