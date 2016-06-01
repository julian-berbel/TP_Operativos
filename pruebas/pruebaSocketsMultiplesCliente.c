/*
 * pruebas2.c
 *
 *  Created on: 31/5/2016
 *      Author: utnso
 */

#include <stdio.h>
#include "cliente.h"

int main(){

	int unSocketCliente = crear_socket_cliente("127.0.0.1", "10349");

	getchar();

	int otroSocketCliente = crear_socket_cliente("127.0.0.1", "10349");

	getchar();

	char* mensaje = "Hola k ase";

	enviar_string(unSocketCliente, mensaje);

	getchar();

	mensaje = "pa k kieres saber eso jaja slds";

	enviar_string(otroSocketCliente, mensaje);

	return 0;
}
