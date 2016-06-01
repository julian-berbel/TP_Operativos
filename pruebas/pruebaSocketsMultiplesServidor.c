/*
 * pruebas.c
 *
 *  Created on: 31/5/2016
 *      Author: utnso
 */

#include <stdio.h>
#include "servidor.h"

int* sockets;
int cantidadDeSockets = 0;
fd_set* bagDeSockets;

void baggearSocket(int unSocket){
	cantidadDeSockets++;
	sockets = realloc(sockets, sizeof(int) * cantidadDeSockets);
	sockets[cantidadDeSockets - 1] = unSocket;
}

int mayorSocket(){
	int mayor = -1;
	int i;
	for(i = 0; i < cantidadDeSockets; i++){
		if(sockets[i] > mayor) mayor = sockets[i];
	}

	return mayor;
}

int main(){

	bagDeSockets = malloc(sizeof(fd_set));

	int socketServer = crear_socket_servidor("127.0.0.1", "10349");

	int conexionRecibida = recibirConexion(socketServer);

	int i;

	char* buffer;

	FD_ZERO(bagDeSockets);

	baggearSocket(conexionRecibida);

	conexionRecibida = recibirConexion(socketServer);

	baggearSocket(conexionRecibida);

	printf("conectado!\n");

	int n = mayorSocket() + 1;

	while(1){
		for(i = 0; i < cantidadDeSockets; i++){
			FD_SET(sockets[i], bagDeSockets);
		}
		select(n, bagDeSockets, NULL, NULL, NULL);

		for(i = 0; i < cantidadDeSockets; i++){
			if (FD_ISSET(sockets[i], bagDeSockets)) {
				buffer = recibir_string_generico(sockets[i]);
				printf("Recibido \"%s\", en el socket %d\n", buffer, i);
				free(buffer);
			}
		}
		FD_ZERO(bagDeSockets);
	}

	for(i = 0; i <= cantidadDeSockets; i++){
		close(sockets[i]);
	}
	close(socketServer);
	return 0;
}


