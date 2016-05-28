#ifndef UMC_H_
#define UMC_H_

#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/config.h>
#include"servidor.h"
#include"cliente.h"

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
t_log* logger;
t_log* logger_pantalla;
int socket_swap;

void abrirConfiguracion();
void cerrar_todo();
void *funcion_nucleo(void *argumento);
void *funcion_cpu(void *argumento);

void inicializar(int id_programa,int paginas_requeridas, char* programa);
void finalizar(int num_programa);
int leer_pagina(int num_pagina, int offset,size_t t);
void escribir_pagina(int num_pagina, int offset, size_t t, char *buffer);


typedef struct{
	int marco;
	int bit_uso;
	int modificado;
} tabla_paginas;

typedef struct{
	int pagina;
	int marco;
} TLB;


int* memoria;
int *marcos_libres;
tabla_paginas* tabla_procesos[1000];
TLB *tlb;

void crear_tabla_de_paginas(int idp, int paginas_requeridas);
int obtener_marco(int idp,int numero_pagina);
void escribir_marco_en_TP(int idp,int pagina, int marco);
void marco_ocupado(int num_marco);
void modificar_retardo(int ret);
void cambiar_proceso_activo(int proceso);
void modificar_bit_uso(int idp,int num_pagina);
void modificar_bit_modificado(int idp, int num_pagina);
char* serializar1(int operacion,int num_pagina,int offset,size_t t);
char* serializar2(int operacion, int id_programa, int paginas_requeridas, char* programa);
int buscar_en_tlb(int num_pagina);

#endif
