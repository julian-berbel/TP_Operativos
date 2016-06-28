#ifndef CPU_H_
#define CPU_H_

#include "cliente.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include "interfazUMC.h"
#include "interfazNucleo.h"
#include "pcb.h"
#include <signal.h>

#define RUTA_LOG "/home/utnso/cpu.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/cpu/config/cpu.config"

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
void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo);
void wait(t_nombre_semaforo identificador_semaforo);
void signal_primitiva(t_nombre_semaforo identificador_semaforo);

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
		.AnSISOP_entradaSalida			= entradaSalida,
};
AnSISOP_kernel kernel_functions = {
		.AnSISOP_wait                   = wait,
		.AnSISOP_signal                 = signal_primitiva,
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
int socket_nucleo;
sig_atomic_t flagTerminar;
int quantum_definido;
int pcb_bloqueado;
int pcb_finalizar;

void abrirConfiguracion();
void cerrar_todo();
char* obtener_instruccion(t_PCB * pcb);
char* pedir_bytes_umc(int num_pagina, int offset, int tamanio);
void enviar_bytes_umc(int num_pagina, int offset, int tamanio, char* buffer);

void cargarPCB(t_PCB* pcb, int quantum);

void ciclosDeQuantum();

void ejecutarInstruccion();

void terminar();

void continuarEjecucion(int quantum);

void desalojar();

#endif
