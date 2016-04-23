/*
 * consola.c
 *
 *  Created on: 19/4/2016
 *      Author: utnso
 */

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
char *puertoNucleo;
char *ip;

int crear_socket_cliente(char * ip, char * puerto);
int enviar_string(int socket, char * mensaje);

int main(){
	abrirConfiguracion();
	char * mensaje = malloc(100);//Defino char* y pido 100 bytes de memoria
	    memset(mensaje, '\0', 100); //Seteo la memoria
	   // printf("%s", ip);
	 //   printf("%s", puertoNucleo);
	    strcpy(mensaje, "Hola\n"); //Le agrego al char* el mensaje a mandar
	    int socket_cliente = crear_socket_cliente("127.0.0.1", "5000"); //Creo un socket cliente (connect)
	    int e = enviar_string(socket_cliente, mensaje); //Envio el string
		close(socket_cliente); //Cierro socket.
		free(mensaje); //Libero mensaje enviado

	return 0;
}
int abrirConfiguracion(){
	t_config* configuracion= config_create("/home/utnso/tp-2016-1c-Hellfish-Group/consola/config/consola.config");
			ip = config_get_string_value(configuracion, "IP_NUCLEO");
			puertoNucleo= config_get_string_value(configuracion, "PUERTO_NUCLEO");
			config_destroy(configuracion);
			return 0;
}

int crear_socket_cliente(char * ip, char * puerto){
    //El proceso es muy similar al de crear un servidor.
    //La explicacion de varias de las funciones esta ahí
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
    /* connect() toma el descriptor devuelto por socket, le asigna un puerto cualquiera (no se sabe)
     *que puede ir cambiando con el curso de la ejecucion (no está bind()) y le dice que se conecte
     *a la IP y puerto pasados, osea la del servidor.
    */
    if (estado == -1){ //Checkea errores
        perror("Error conectando el socket");
        freeaddrinfo(infoServer);
        return -1;
    }
    freeaddrinfo(infoServer); //Libera estructura addrinfo
    return descriptorArchivo; //Devuelve el descriptor conectado
}

int enviar_string(int socket, char * mensaje){ //Recibe el socket conectado y un mensaje
	int check_error = send(socket, mensaje, 100, 0);
        /*A la funcion send se le pasa el socket conectado, el buffer donde se encuentra el mensaje
         *y su longitud. Y denuevo el flag 0.
        */
	if(check_error < 0){ //Chequea errores
	return 1;
	}
    return 0;
}

