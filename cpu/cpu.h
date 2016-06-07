#ifndef CPU_H_
#define CPU_H_

#include "cliente.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include <parser/parser.h>
#include <commons/collections/list.h>
#include "interfazUMC.h"

#define RUTA_LOG "/home/utnso/cpu.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/cpu/config/cpu.config"

typedef struct{
	int pid;
	int programCounter;
	int cantidadPaginas;
	int cantidadInstrucciones;
	int** indiceCodigo;
	int tamanioIndiceEtiquetas;
	char* indiceEtiquetas;
	t_list* indiceStack;
} t_PCB;

typedef struct{
	int pagina;
	int offset;
	int size;
} pos_mem;

typedef struct{
	t_list* args; //Lista de pos_mem's. Cada posicion representa un argumento en el orden de la lista
	t_list* vars; // Lista de variables(struct)
	int dir_retorno;
	pos_mem* var_retorno;
} nodo_stack;

typedef struct{
	char nombre_var;
	pos_mem* dir_var;
} t_variable;

//Defino funciones de primitivas

t_puntero definirVariable(t_nombre_variable variable);
t_puntero obtenerPosicionVariable(t_nombre_variable variable);
t_valor_variable dereferenciar(t_puntero puntero);
void asignar(t_puntero puntero, t_valor_variable variable);
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable);
t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor);
void irAlLabel(t_nombre_etiqueta etiqueta);
void llamarSinRetorno(t_nombre_etiqueta etiqueta);
void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);
void finalizar();
void retornar(t_valor_variable retorno);
void imprimir(t_valor_variable valor);
void imprimirTexto(char* texto);
void wait(t_nombre_semaforo identificador_semaforo);
void signal(t_nombre_semaforo identificador_semaforo);

AnSISOP_funciones functions = {
		.AnSISOP_definirVariable		= definirVariable,
		.AnSISOP_obtenerPosicionVariable= obtenerPosicionVariable,
		.AnSISOP_dereferenciar			= dereferenciar,
		.AnSISOP_asignar				= asignar,
		.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
		.AnSISOP_asignarValorCompartida = asignarValorCompartida,
		.AnSISOP_irAlLabel              = irAlLabel,
		.AnSISOP_llamarSinRetorno       = llamarSinRetorno,
		.AnSISOP_llamarConRetorno       = llamarConRetorno,
		.AnSISOP_finalizar              = finalizar,
		.AnSISOP_retornar               = retornar,
		.AnSISOP_imprimir				= imprimir,
		.AnSISOP_imprimirTexto			= imprimirTexto,

};
AnSISOP_kernel kernel_functions = {
		.AnSISOP_wait                   = wait,
		.AnSISOP_signal                 = signal,
};

t_config* configuracionCPU;
char* ipNucleo;
char* puertoNucleo;
char* ipUMC;
char* puertoUMC;
int socket_umc;
t_log* logger;
t_log* logger_pantalla;
int tamanio_pagina;
t_PCB *pcb_actual;
const int CONTENIDO_VARIABLE = 20; //Borrar cuando esten todas las primitivas
const int POSICION_MEMORIA = 0x10; //Borrar cuando esten todas las primitivas

void abrirConfiguracion();
void cerrar_todo();
char* obtener_instruccion(t_PCB * pcb);
char* pedir_bytes_umc(int num_pagina, int offset, int tamanio);

#endif
