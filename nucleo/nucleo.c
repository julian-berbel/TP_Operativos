/*
 * nucleo.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */
#include<sys/epoll.h> //Facilita el manejo de errores (se usa en algunas funciones)
#include<stdio.h> //Para usar perror()
#include<errno.h> //Para usar la variable errno
#include<sys/types.h> //Importa tipos de datos requeridos por el socket
#include<sys/socket.h> //Importa las funciones necesarias del socket más algunas estructuras
#include<netdb.h> //Para que tenga las definiciones de addrinfo
#include<fcntl.h> //Para que pueda cambiar el flag de un socket a non_blocking
#include<string.h> //Para importar memset()
#include<unistd.h> //Para importar el close()
#include<stdlib.h> //Para usar malloc() y calloc()
#include<netinet/in.h> //Contiene mas definiciones necesarias para addrinfo
#include<commons/string.h> //Para el uso de algunas de las funciones String de las commons
#include<commons/config.h>
#include<stdio.h>
int abrirConfiguracion();
int crear_socket_servidor(char *ip,char *puerto); //Funcion que va a crear el socket servidor que recibe la IP y Puerto
char * recibir_string(int socket_aceptado);
char *puertoNucleo;
char *ip_prog;

int main(){
	abrirConfiguracion();
	struct sockaddr_storage their_addr;
	        /*Defino el puntero their_addr que apunta a una estructura Sockaddr_storage que es la que va a
	         *contener la informacion de los socket que se conecten al servidor.
	         * Ver http://beej.us/guide/bgnet/output/html/multipage/ipstructsdata.html (Estructura de sockaddr_storage)
	         * Ver http://beej.us/guide/bgnet/output/html/multipage/acceptman.html (manual del accept())
	        */
		socklen_t addr_size; //Es una estructura que, básicamente, tiene un int para guardar longitud (Se ve mas adelante).
		int socket_servidor = crear_socket_servidor(ip_prog, puertoNucleo);
	        /*Llamada a la funcion para crear el socket Y GUARDO EL VALOR INT QUE DEVUELVE
	         *"127.0.0.1 es una direccion IP standar que significa que el proceso Servidor esta corriendo en la maquina local
	         *"8080" es un puerto elegido al azar. Siempre tienen que ser mayor que 1024, sino el SO te lo niega.
	        */
		int estado = listen(socket_servidor, 5);
	        /*Hago que el socket_servidor se ponga a escuchar conexiones entrantes
	         *El valor 5 es para darle una longitud a la cola de conexiones entrantes que puede tener
	         *Si entran más, las rechaza.
	         *IMPORTANTE= el proceso se bloquea en esta funcion hasta que entre alguna conexion y
	         *se acepte con la funcion accept().
		*/
	        if(estado == -1){ //La funcion listen devuelve -1 si hubo algun error. Con el "if" checkeo eso.
		    printf("Error al poner el servidor en listen\n");
	            close(socket_servidor); //Cierro el socket en caso de error
	            return 1;//Salgo del main
		}
		if(estado == 0){ //Si funciono bien el listen devuelve 0
		    printf("Se puso el socket en listen\n");
		}

	        addr_size = sizeof(their_addr);
	        /*Le digo a addr_size que almacene el tamaño de la estructura their_addr (sockaddr_storage)
	         *Esto es necesario porque despues el accept lo necesita
	        */
		int socket_aceptado = accept(socket_servidor, (struct sockaddr *)&their_addr, &addr_size);
		/*Una vez que se recibe alguna conexion se la accepta y se crea un nuevo socket para manejar
	         *esa nueva conexion, sin perder el socket_servidor de escucha.
	         *Fijarse bien los parametros de esta funcion. Aca se usa el their_addr y el addr_size
		*/
	        if (socket_aceptado == -1){ //Checkeo de errores. accept() devuelve -1 en error
	            close(socket_servidor);
		    printf("Error al aceptar conexion\n");
	            return 1;
		}
	char * handshake = recibir_string(socket_aceptado);

	printf("%s\n",handshake);
	free(handshake);
	close(socket_servidor);
	close(socket_aceptado);
	return 0;
}



int abrirConfiguracion(){
	t_config* configuracion= config_create("/home/utnso/tp-2016-1c-Hellfish-Group/nucleo/config/nucleo.config");
			puertoNucleo = config_get_string_value(configuracion, "PUERTO_PROG");
			ip_prog= config_get_string_value(configuracion, "IP_NUCLEO");
			return 0;
}

int crear_socket_servidor(char *ip, char *puerto){
    //Funcion que recibe la ip y el puerto que va a tener el proceso servidor para escuchar mensajes
    int descriptorArchivo, estado; //Int necesarios para guardar los resultados de socket()
    struct addrinfo hints, *infoServer, *n;
    /*hints es una variable de una estructura addrinfo que se usa para guardar informacion sobre los socket.
     *Ver http://beej.us/guide/bgnet/output/html/multipage/ipstructsdata.html (Estructuras)
     *Recordar que las estructuras en C son como un conjunto de valores que se mueven juntos,
     *Como cuando en Haskell(paradigmas) definias un nuevo tipo de dato, que tenian varias cosas.
     *infoServer y n son punteros (por eso el *) a la estructura addrinfo.
    */

    memset(&hints,0,sizeof (struct addrinfo));
    /* memset recibe tres parametros: una porcion de memoria (en este caso la estructura hints),
     * un char o int (en este caso 0),
     * y un int que generalmente es el largo en bytes de la porcion de memoria pasada en el primer parametro.
     * Hace que todos los bytes de la porcion de memoria pasada en el primer parametro valgan el char o int
     * del segundo parametro (0). Generalmente se usa como inicializacion.
    */
    hints.ai_family = AF_UNSPEC;
    //Cargo en la estructura que la IP que va usar el socket puede ser IPv4 o IPv6 (Casi siempre se usa IPv4)
    hints.ai_socktype = SOCK_STREAM;
    /*Cargo en la estructura que el tipo de socket es STREAM.
     *Es lo que mayormente se usa ya que STREAM asegura que la info enviada llega en orden y sin errores.
     *El otro tipo es DGRAM (datagram) que es mas rapido pero no asegura el orden ni que se reciba todo
     *lo que se envio.
    */

    if ((estado = getaddrinfo(ip, puerto, &hints, &infoServer)) != 0){
    /*Hago el getaddrinfo para cargar el puntero *infoServer con varios ficheros que pueden o no ser validos
     *y chequeo errores
    */
        fprintf(stderr, "Error en getaddrinfo: %s", gai_strerror(estado));
        return -1;
    }

    for(n = infoServer; n != NULL; n = n->ai_next){
    /*Hago que que el puntero n apunte a lo mismo que infoServer y recorro la lista hasta que la funcion socket()
     *devuelva un valor distinto de -1 lo que significa que encontro un fichero valido. Entonces sale
     *con "break" del for.
    */
        descriptorArchivo = socket(n->ai_family, n->ai_socktype, n->ai_protocol);
        if(descriptorArchivo != -1)
            break;
    }
    if(descriptorArchivo == -1){
        perror("Error al crear el socket");
        freeaddrinfo(infoServer);
        return -1;
    }
    int si = 1;
            if(setsockopt(descriptorArchivo,SOL_SOCKET,SO_REUSEADDR,&si,sizeof(int)) == -1){
            /*Hago el setsockopt() para cambiar el flag del socket a SO_REUSEADDR que hace que el puerto
             *asignado al socket NO se pueda usar por otro proceso, incluso aunque el proceso que lo
             *solicito se cierre. Es para evitar problemas con el puerto ante caida de servidor y vuelta en marcha.
            */
                perror("Error en setsockopt");
                close(descriptorArchivo);
                freeaddrinfo(infoServer);
                return -1;
            }
    if (bind(descriptorArchivo, n->ai_addr, n->ai_addrlen) == -1){
            /*Con bind() se hace efectiva la asigancion del puerto al socket y hace que ese puerto sea
             *siempre el mismo hasta que se cierre el socket.
            */
                perror("Error bindeando el socket");
                close(descriptorArchivo);
                freeaddrinfo(infoServer);
                return -1;
            }
    freeaddrinfo(infoServer); //LIBERO la estructura addrinfo que ya no es necesaria
    return descriptorArchivo;
    //Devuelvo el fichero (int) que me dio la funcion socket(), que ademas tiene el bind
}

char * recibir_string(int socket_aceptado){ //Esta funcion solo recibe un socket
	char *error = string_new(); //Defino string de error
	string_append(&error, "fail"); //Hago que el string contenga "fail" (funcion de las commons)
	char *mensaje = malloc(100); //Defino un char* y le asigno 100 bytes de memoria
        char *mensaje_2 = malloc(100); //Defino otro char*
	memset(mensaje, '\0', 100); //Uso memset para inicializar la memoria con '\0'
	int error_check = recv(socket_aceptado, mensaje, 100, 0);
        /*Hago el recv con el socket y le digo que guarde lo que reciba en buffer "mensaje"
         *ademas le digo que ese buffer tiene tamaño 100. El 0 es un flag.
         *IMPORTANTE = Puede que la funcion recv no reciba de una todos los bytes que le fueron enviados.
         *Puede recibir todo el mensaje o bien una parte. El programador controla que se haya recibido todo
        */
	if(error_check <= 0){ //Checkeo que recv no haya dado error (<0)
                free(mensaje_2);
		free(mensaje);
		return error;
	}
        while(strcmp(mensaje,"Hola\n") != 0){
        //Chequeo que el mensaje recibido sea el esperado. Sino vuelvo a hacer los recv necesarios
            memset(mensaje_2, '\0', 100);
            error_check = recv(socket_aceptado, mensaje_2, 100, 0);
           	if(error_check <= 0){
                    free(mensaje_2);
		    free(mensaje);
		    return error;
	        }
            string_append(&mensaje, mensaje_2);//Le agrego el contenido de mensaje_2 al mensaje (principal)
            }
        free(error); //Libero error
        free(mensaje_2); //Libero mensaje_2
        return mensaje;
}
