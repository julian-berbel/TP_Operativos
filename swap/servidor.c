#include "servidor.h"

int crear_socket_servidor(char *ip, char *puerto){
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
        if(descriptorArchivo != -1) break;
    }

    if(descriptorArchivo == -1){
        perror("Error al crear el socket");
        freeaddrinfo(infoServer);

        return -1;
    }

    int si = 1;

    if(setsockopt(descriptorArchivo,SOL_SOCKET,SO_REUSEADDR,&si,sizeof(int)) == -1){
    	perror("Error en setsockopt");
        close(descriptorArchivo);
        freeaddrinfo(infoServer);

        return -1;
    }

    if (bind(descriptorArchivo, n->ai_addr, n->ai_addrlen) == -1){
    	perror("Error bindeando el socket");
        close(descriptorArchivo);
        freeaddrinfo(infoServer);

        return -1;
    }

    freeaddrinfo(infoServer);

    return descriptorArchivo;
}

char* recibir_string(int socket_aceptado){
	char *error = string_new();
	string_append(&error, "fail");
	char *mensaje = malloc(100);
    char *mensaje_2 = malloc(100);
	memset(mensaje, '\0', 100);
	int error_check = recv(socket_aceptado, mensaje, 100, 0);

	if(error_check <= 0){
        free(mensaje_2);
		free(mensaje);
		return error;
	}

    while(strcmp(mensaje,"Hola\n") != 0){
    	memset(mensaje_2, '\0', 100);
    	error_check = recv(socket_aceptado, mensaje_2, 100, 0);

    	if(error_check <= 0){
    		free(mensaje_2);
		    free(mensaje);
		    return error;
    	}

    	string_append(&mensaje, mensaje_2);
    }
        free(error);
        free(mensaje_2);

        return mensaje;
}

char* recibir_string_generico(int socket_aceptado){
	char* mensaje = string_new();
	char* acumulador = malloc(101);

	int aux;
	int aux2 = 0;

	while((aux = recv(socket_aceptado, acumulador, 100, 0)) > 0){
		string_append(&mensaje, acumulador);
		aux2 += aux;
	}

	printf("%d", aux2);
	free(acumulador);
	return mensaje;
}
