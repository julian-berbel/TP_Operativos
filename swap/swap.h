#ifndef SWAP_H_
#define SWAP_H_

#include <stdio.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <commons/config.h>
#include "servidor.h"
#include "binFile.h"
#include "interfazUMC.h"
#include <signal.h>

#define RUTA_LOG "/home/utnso/swap.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/swap/config/swap.config"

typedef int Bool;
#define true 1
#define false 0

typedef struct{
	int pid;
	int pagina;
	//int posicion; pagina = posicion
	//int offset;
	//int bit_uso;
} t_proceso;

typedef struct{
	int pagina;
	int bit_uso;
} t_swap;

t_list* espacioTotal;
t_list* listaDeProcesos;

t_config* configuracion_swap;
char* puertoSwap;
char* ipSwap;
int cant_paginas;
t_log* logger;
t_log* logger_pantalla;
sig_atomic_t flagTerminar = 0;
int socket_umc;

void abrirConfiguracion();
void cerrar_todo();

void inicializar(int id_programa,int paginas_requeridas, char* programa);
void finzalizar(int id_programa);

int cant_pags_disponibles();
int hayEspacio(int paginas_requeridas);
void agregarProcesoALista(int id_programa,int paginas_requeridas);
int tamanioListaSwap();

int hayQueCompactar(int paginas_requeridas);
int estanPaginasContinuas(t_list* espaciosLibres, int paginas_requeridas);
Bool estaUtilizado();
Bool noEstaUtilizado();
Bool noSonIguales();
void compactar();
void destruirElemento(t_swap*);
t_list* sacarRepetidos(t_list* lista);
t_list* sacarRepetidos2(t_list* lista);

void terminar();
void leer_pagina(int id_programa, int num_pagina);
void escribir_pagina(int id_programa, int num_pagina, char* buffer);
void recorrerYModificarArchivoYListas();

#endif
