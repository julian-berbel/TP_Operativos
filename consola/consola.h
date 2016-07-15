#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "cliente.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include <sys/stat.h>
#include <signal.h>
#include "interfazNucleo.h"

#define RUTA_LOG "/home/utnso/consola"
#define RUTA_PROGRAMA argumentos[2]

t_config* configuracionConsola;
char* ipNucleo;
char* puertoNucleo;
t_log* logger;
t_log* logger_pantalla;
int socket_nucleo;

int flagTerminar = 0;

void abrirConfiguracion(char* ruta);
void cerrar_todo();
int archivoExiste();
char* leerArchivo();

void imprimir(char* mensaje);

void senialTerminar(int n);

void terminar();

char* devolver_ruta_log();

#endif
