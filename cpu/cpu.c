/*
 * cpu.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */

#include <commons/config.h>
#include <stdio.h>
#include<sys/epoll.h>
#include<stdio.h> //Para usar perror()
#include<errno.h> //Para usar la variable errno
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h> //Para que tenga las definiciones de addrinfo
#include<fcntl.h> //Para que pueda cambiar el flag de un socket a non_blocking
#include<string.h> //Para importar memset()
#include<unistd.h> //Para importar el close()
#include<stdlib.h> //Para usar malloc() y calloc()
#include<netinet/in.h> //Contiene mas definiciones necesarias para addrinfo
#include<commons/string.h>
#include <commons/config.h>
#include <stdio.h>
int abrirConfiguracion();
char *puerto_nucleo;
char *ip_nucleo;
char *puerto_umc;
char *ip_umc;

int crear_socket_cliente(char * ip, char * puerto);
int enviar_string(int socket, char * mensaje);

int main(){
	abrirConfiguracion();

	 char * mensaje_nucleo = malloc(100);
	        memset(mensaje_nucleo, '\0', 100);
	        strcpy(mensaje_nucleo, "Hola\n");
	 char * mensaje_umc = malloc(100);
	        memset(mensaje_umc, '\0', 100);
	        strcpy(mensaje_umc, "Hola\n");

	 int socket_cliente_nucleo = crear_socket_cliente(ip_nucleo, puerto_nucleo);
	 int e = enviar_string(socket_cliente_nucleo, mensaje_nucleo);

	 /*int socket_cliente_umc = crear_socket_cliente(ip_umc, puerto_umc);
	 int r = enviar_string(socket_cliente_umc, mensaje_umc);
		//cierro los sockets
		close(socket_cliente_nucleo);
		/*close(socket_cliente_umc);*/
		//libero mensajes
		free(mensaje_nucleo);
		/*free(mensaje_umc);*/


	return 0;
}


int abrirConfiguracion(){
	t_config* configuracion= config_create("/home/utnso/tp-2016-1c-Hellfish-Group/cpu/config/cpu.config");
		puerto_nucleo = config_get_string_value(configuracion, "PUERTO_NUCLEO");
		puerto_umc=config_get_string_value(configuracion, "PUERTO_UMC");
		ip_nucleo=config_get_string_value(configuracion, "IP_NUCLEO");
		ip_umc=config_get_string_value(configuracion, "IP_UMC");
	return 0;
}

int crear_socket_cliente(char * ip, char * puerto){
    int descriptorArchivo, estado;
    struct addrinfo hints, *infoServer, *n;

    memset(&hints,0,sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((estado = getaddrinfo(ip, puerto, &hints, &infoServer)) != 0){
        fprintf(stderr, "Error en getaddrinfo: %s", gai_strerror(estado));
        return -1;
    }

    for(n = infoServer; n != NULL; n = n->ai_next){
        descriptorArchivo = socket(n->ai_family, n->ai_socktype, n->ai_protocol);
        if(descriptorArchivo != -1)
            break;
    }
    if(descriptorArchivo == -1){
        perror("Error al crear el socket");
        freeaddrinfo(infoServer);
        return -1;
    }
    estado = connect(descriptorArchivo, n->ai_addr, n->ai_addrlen);

    if (estado == -1){ //Checkea errores
        perror("Error conectando el socket");
        freeaddrinfo(infoServer);
        return -1;
    }
    freeaddrinfo(infoServer); //Libera estructura addrinfo
    return descriptorArchivo; //Devuelve el descriptor conectado
}

int enviar_string(int socket, char * mensaje){
	int check_error = send(socket, mensaje, 100, 0);

	if(check_error < 0){ //Chequea errores
	return 1;
	}
    return 0;
}


