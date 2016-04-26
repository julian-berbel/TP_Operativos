/*
 * servidor.h
 *
 *  Created on: 24/4/2016
 *      Author: utnso
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include "socketComms.h"

int crear_socket_servidor(char *ip, char *puerto);

int recibirConexion(int socket_servidor);

#endif /* SERVIDOR_H_ */
