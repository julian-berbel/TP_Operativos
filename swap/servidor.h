/*
 * servidor.h
 *
 *  Created on: 24/4/2016
 *      Author: utnso
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <sys/epoll.h> //Facilita el manejo de errores (se usa en algunas funciones)
#include <stdio.h> //Para usar perror()
#include <errno.h> //Para usar la variable errno
#include <sys/types.h> //Importa tipos de datos requeridos por el socket
#include <sys/socket.h> //Importa las funciones necesarias del socket más algunas estructuras
#include <netdb.h> //Para que tenga las definiciones de addrinfo
#include <fcntl.h> //Para que pueda cambiar el flag de un socket a non_blocking
#include <string.h> //Para importar memset()
#include <unistd.h> //Para importar el close()
#include <stdlib.h> //Para usar malloc() y calloc()
#include <netinet/in.h> //Contiene mas definiciones necesarias para addrinfo
#include <commons/string.h> //Para el uso de algunas de las funciones String de las commons
#include <commons/config.h>
#include <stdio.h>

int crear_socket_servidor(char *ip, char *puerto);

char* recibir_string(int socket_aceptado);

char* recibir_string_generico(int socket_aceptado);

#endif /* SERVIDOR_H_ */
