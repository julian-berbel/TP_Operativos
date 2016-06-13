#ifndef UMC_H_
#define UMC_H_

#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/config.h>
#include"servidor.h"
#include"cliente.h"
#include "interfazNucleoCPU.h"
#include "interfazSwap.h"

#define RUTA_LOG "/home/utnso/umc.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/umc/config/umc.config"

int id_proceso_activo;
t_config* configuracionUMC;
char* puertoUmc;
char* ipUmc;
char* ipSwap;
char* puertoSwap;
int marcos;
int marco_size;
int marco_x_proc;
int entradas_tlb;
int tlb_habilitada;
int retardo;
char* alg_reemplazo;
t_log* logger;
t_log* logger_pantalla;
int socket_swap;

void abrirConfiguracion();
void cerrar_todo();
void *funcion_nucleo(void *argumento);
void *funcion_cpu(void *argumento);

void inicializar(int id_programa,int paginas_requeridas, char* programa);
void finalizar(int num_programa);
void leer_pagina(int num_pagina, int offset,size_t t);
void escribir_pagina(int num_pagina, int offset, size_t t, char *buffer);

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
tabla_paginas tabla_procesos[50][20];
TLB *tlb;
int cant_paginas_procesos[50];
int procesos_ocupados[50];
int punteros_clock[50];


void crear_tabla_de_paginas(int idp, int paginas_requeridas);
int obtener_marco(int idp,int num_pagina);
int obtener_marco_tabla_paginas(int idp,int numero_pagina);
int obtener_marco_tlb(int idp,int num_pagina);
void escribir_marco_en_TP(int idp,int pagina, int marco);
void escribir_marco_en_tlb(int idp,int num_pagina,int marco);
void marco_ocupado(int num_marco);
void marco_desocupado(int num_marco);
void cambiar_proceso_activo(int proceso);
void modificar_bit_uso(int idp,int num_pagina);
void modificar_bit_modificado(int idp, int num_pagina);
void escribir_posicion_memoria(int posicion,size_t tamanio,char *buffer);
char* leer_posicion_memoria(int posicion, size_t tamanio);
int buscar_indice_libre_tlb();
void aumentar_uso_tlb(int idp,int num_pagina);
int buscar_indice_menos_accedido_tlb();
void flush (int idp);
//operaciones de consola
void modificar_retardo(int ret);
void dump_est_proceso(int idp, const char*);
void dump_est_gen();
void dump_cont_proceso(int idp);
void dump_cont_gen();
void flush_tlb();
void flush_memory(int idp/*,int cantidad_paginas*/);
void terminar();
void reconocer_comando(char *comando, char* param);
int buscar_marco_libre();
int cant_paginas_asignadas(int idp);
int reemplazar_MP(int idp,int num_pagina);
int  buscar_pagina_victima(int idp);

#endif
