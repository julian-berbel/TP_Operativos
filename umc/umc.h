#ifndef UMC_H_
#define UMC_H_

#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/config.h>
#include"servidor.h"
#include"cliente.h"
#include <string.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include "interfazNucleoCPU.h"
#include "interfazSwap.h"


#define RUTA_LOG "/home/utnso/umc.log"

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
char* alg_reemplazo;
t_log* logger;
t_log* logger_pantalla;
int socket_swap;
int ref_tlb;
int cant_reemplazos_tlb;
int cant_reemplazos_memoria;

pthread_mutex_t lock;

pthread_t hiloReceptor;
pthread_t hiloInterpreteConsola;

t_list* clientes;
_Bool flagTerminar;
sem_t semTerminar;

typedef struct{
	pthread_t* hiloCliente;
	int socket;
	int proceso_activo;
} t_cliente;

int socket_servidor;

void abrirConfiguracion(char* ruta);
void inicializar_estructuras();
void cerrar_todo();
void *funcion_nucleo(void *argumento);
void *funcion_cpu(void *argumento);

void inicializar(int id_programa,int paginas_requeridas, char* programa, void* cliente);
void finalizar(int num_programa);
void leer_pagina(int num_pagina, int offset,size_t t, void* cliente);
void escribir_pagina(int num_pagina, int offset, size_t t, char *buffer, void* cliente);

typedef struct{
	int marco;
	int presencia;
	int bit_uso;
	int modificado;
} tabla_paginas;

typedef struct{
	int idp;
	int pagina;
	int marco;
	int uso;
} TLB;


char* memoria;
int *marcos_libres;
tabla_paginas tabla_procesos[100][100];
TLB *tlb;
int cant_paginas_procesos[100];
int procesos_ocupados[100];
int punteros_clock[100];


void crear_tabla_de_paginas(int idp, int paginas_requeridas);
int obtener_marco(int idp,int num_pagina);
int obtener_marco_tabla_paginas(int idp,int numero_pagina);
int obtener_marco_tlb(int idp,int num_pagina);
void escribir_marco_en_TP(int idp,int pagina, int marco);
void escribir_marco_en_tlb(int idp,int num_pagina,int marco);
void marco_ocupado(int num_marco);
void marco_desocupado(int num_marco);
void cambiar_proceso_activo(int pid, void* cliente);
void escribir_posicion_memoria(int posicion,size_t tamanio,char *buffer);
char* leer_posicion_memoria(int posicion, size_t tamanio);
int buscar_indice_libre_tlb();
int buscar_indice_menos_accedido_tlb();
void flush (int idp);
void copiar_pagina_en_memoria(int idp,int num_pagina,char* contenido_pagina);
void vaciar_marco(int marco);
void borrar_marcos(int idp);
void imprimir_tlb(char*);
void imprimir_TP(int idp,char*estado);
void actualizar_referencia(int idp, int num_pagina);

//operaciones de consola
void modificar_retardo(int ret);
void dump_est_proceso(int idp, const char* nombreArchivo);
void dump_est_gen();
void dump_cont_proceso(int idp, const char* nombreArchivo);
void dump_cont_gen();
void flush_tlb();
void flush_memory(int idp/*,int cantidad_paginas*/);
void terminar();
void reconocer_comando(char *comando, char* param);
int buscar_marco_libre();
int cant_paginas_asignadas(int idp);
int reemplazar_MP(int idp,int num_pagina);
void sacar_pagina_de_tlb(int idp,int pagina);
int  buscar_pagina_victima(int idp);

#endif
