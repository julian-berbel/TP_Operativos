#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "cliente.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include <sys/stat.h>
#include <signal.h>

#define RUTA_LOG "/home/utnso/consola.log"
#define RUTA_CONFIG "/home/utnso/tp-2016-1c-Hellfish-Group/consola/config/consola.config"
#define RUTA_PROGRAMA argumentos[1]

t_config* configuracionConsola;
char* ipNucleo;
char* puertoNucleo;
t_log* logger;
t_log* logger_pantalla;
int socket_nucleo;

void abrirConfiguracion();
void cerrar_todo();
int archivoExiste();
char* leerArchivo();

#endif
