#include "umc.h"

void threadEscucha(t_cliente* cliente) {
	_Bool buscarCliente(void* elementoDeLaLista){
		return elementoDeLaLista == cliente;
	}
	int socket = cliente->socket;
	enviar(socket, &marco_size, sizeof(int));

	void* mensaje;

	while (!flagTerminar) {
		mensaje = recibir(socket);
		if (!mensaje) {
			printf("Cliente desconectado!\n");
			list_remove_by_condition(clientes, (void*) buscarCliente);
			shutdown(cliente->socket, 0);
			free(cliente->hiloCliente);
			free(cliente);
			break;
		}
		procesarMensaje(mensaje, cliente);
	}

	close(socket);
}

void crearThreadEscucha(int conexionRecibida) {
	pthread_t* hiloEscucha = malloc(sizeof(pthread_t));

	t_cliente* cliente = malloc(sizeof(t_cliente));
	cliente->socket = conexionRecibida;

	cliente->hiloCliente = hiloEscucha;

	list_add(clientes, (void*) cliente);

	pthread_create(hiloEscucha, NULL, (void*) threadEscucha, (void*) cliente);
}

void threadReceptor(void* param) {
	int socketServer = *((int*) param);

	int conexionRecibida = recibirConexion(socketServer);

	while (!flagTerminar) {
		crearThreadEscucha(conexionRecibida);
		conexionRecibida = recibirConexion(socketServer);
	}

	close(socketServer);
}

void threadInterpreteConsola() {
	char* mensaje = NULL;
	size_t a = 0;
	int leido;
	char * comandoAEjecutar,*parametro;

	while (!flagTerminar) {
		getline(&mensaje, &a, stdin);
		leido = string_length(mensaje);
		*(mensaje + leido - 1) = '\0';

		 comandoAEjecutar = strtok(mensaje," ");
		 parametro = strtok(0," ");
		 reconocer_comando(comandoAEjecutar,parametro);
	}

	free(mensaje);
}
int main(int cantidadArgumentos, char* argumentos[]) {

 abrirConfiguracion();
 log_info(logger, "Inicia proceso UMC");

 inicializar_estructuras();

 if (pthread_mutex_init(&lock, NULL) != 0)
 {
	 log_info(logger, "Fallo la inicializacion del mutex");
 }

 sem_init(&semTerminar, 0, 0);

 socket_swap = crear_socket_cliente(ipSwap, puertoSwap);
 log_info(logger_pantalla, "UMC y Swap conectados");

 socket_servidor = crear_socket_servidor(ipUmc, puertoUmc);

 clientes = list_create();

 pthread_create(&hiloReceptor, NULL,(void*) threadReceptor, &socket_servidor);

 pthread_create(&hiloInterpreteConsola, NULL,
 (void*) threadInterpreteConsola, NULL);

 sem_wait(&semTerminar);

 //Comienza el cierre del main.

 cerrar_todo();
 return 0;
 }


void inicializar_estructuras(){
	int i;
	 for (i = 0; i < 20; i++) {
	 punteros_clock[i] = 0;	//comienzan apuntando a la pagina 0
	 }
	 //array de disponibilidad de marcos
	 marcos_libres = (int*) malloc(sizeof(int) * marcos);

	 for (i = 0; i < marcos; i++) { //lo inicializo en cero:testeado
	 marcos_libres[i] = 0;
	 }
	 //array con la cantidad de paginas de cada proceso y procesos ocupados

	 for (i = 0; i < 20; i++) { //inicializo en cero
	 cant_paginas_procesos[i] = 0;
	 procesos_ocupados[i] = 0;
	 }

	 //memoria
	 memoria = (char *) malloc(marcos * marco_size * sizeof(char));
	 memset(memoria,' ',marcos * marco_size * sizeof(char));

	 //TLB
	 if (tlb_habilitada){
	 tlb = (TLB*) malloc(entradas_tlb * sizeof(TLB));

	 for (i = 0; i < entradas_tlb; i++) { //inicializo la tlb con idp en -1:testeado
	 tlb[i].idp = -1;
	 tlb[i].marco=-1;
	 tlb[i].pagina=-1;
	 tlb[i].uso=0;
	 }
	 }
}
void abrirConfiguracion() {
	configuracionUMC = config_create(RUTA_CONFIG);
	ipUmc = config_get_string_value(configuracionUMC, "IP_UMC");
	puertoUmc = config_get_string_value(configuracionUMC, "PUERTO");
	ipSwap = config_get_string_value(configuracionUMC, "IP_SWAP");
	puertoSwap = config_get_string_value(configuracionUMC, "PUERTO_SWAP");
	marcos = config_get_int_value(configuracionUMC, "MARCOS");
	marco_size = config_get_int_value(configuracionUMC, "MARCO_SIZE");
	marco_x_proc = config_get_int_value(configuracionUMC, "MARCO_X_PROC");
	entradas_tlb = config_get_int_value(configuracionUMC, "ENTRADAS_TLB");
	tlb_habilitada = config_get_int_value(configuracionUMC, "TLB_HABILITADA");
	retardo = config_get_int_value(configuracionUMC, "RETARDO");
	alg_reemplazo = config_get_string_value(configuracionUMC, "ALG_REEMPLAZO");
	logger = log_create(RUTA_LOG, "UMC", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "UMC", true, LOG_LEVEL_INFO);
}

void destruirCliente(t_cliente* cliente) {
	shutdown(cliente->socket, 0);
	pthread_join(*cliente->hiloCliente, NULL);
	free(cliente->hiloCliente);
	free(cliente);
}

void cerrar_todo() {
	shutdown(socket_servidor, 0);
	pthread_join(hiloReceptor, NULL);
	shutdown(socket_swap, 0);
	fclose(stdin);

	pthread_join(hiloInterpreteConsola, NULL);

	list_destroy_and_destroy_elements(clientes, (void*) destruirCliente);

	close(socket_swap);
	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracionUMC);
	free(memoria);
	if(tlb_habilitada)
	free(tlb);
	free(marcos_libres);
	log_info(logger, "finalizacion del proceso UMC");
}

void inicializar(int id_programa, int paginas_requeridas, char* programa, void* cliente) {
	log_info(logger, "Inicializar proceso %d, consultar a SWAP espacio disponible",id_programa);
	void* mensaje;
	int tamanioMensaje = serializarInicializar(id_programa, paginas_requeridas, programa, &mensaje);
	pthread_mutex_lock(&lock);
	enviar(socket_swap, mensaje, tamanioMensaje);
	free(mensaje);
	char* respuesta = recibir_string_generico(socket_swap);
	enviar_string(((t_cliente*)cliente)->socket, respuesta);
	if(!strcmp(respuesta,"OK")){
	crear_tabla_de_paginas(id_programa, paginas_requeridas); //testeado
	}
	free(respuesta);
	pthread_mutex_unlock(&lock);
}

void finalizar(int id_programa) {
	borrar_marcos(id_programa);
	procesos_ocupados[id_programa] = 0;
	flush(id_programa);
	//avisar al swap para que libere memoria
	void* mensaje;
		int tamanioMensaje = serializarFinalizar(id_programa, &mensaje);
		pthread_mutex_lock(&lock);
		enviar(socket_swap, mensaje, tamanioMensaje);
		pthread_mutex_unlock(&lock);
		free(mensaje);
}

void borrar_marcos(int idp){
	int i;
	int paginas=cant_paginas_procesos[idp];
	for(i=0;i<paginas;i++){
		if(tabla_procesos[idp][i].presencia==1){
			int marco= tabla_procesos[idp][i].marco;
			vaciar_marco(marco);
			marcos_libres[marco]=0;
		}
	}
}

void vaciar_marco(int marco){
	int i;
	for(i=0;i<marco_size;i++){
		memoria[marco*marco_size+i]=' ';
	}
}

void leer_pagina(int num_pagina, int offset, size_t t, void* cpu) {
	int idp=((t_cliente*)cpu)->proceso_activo;
	pthread_mutex_lock(&lock);
	int marco = obtener_marco(idp, num_pagina);
	if (marco !=-1) { //si esta en MP
		log_info(logger, "Pagina %d, encontrada en memoria en el marco %d",num_pagina,marco);
		char* contenido = leer_posicion_memoria(marco * marco_size + offset, t);
		enviar_string(((t_cliente*)cpu)->socket,contenido);
		log_info(logger, "Contenido enviado: %s", contenido);
		free(contenido);

	} else { //no esta en MP
		log_info(logger, "Page Fault, no se encontró la pagina %d en memoria",num_pagina);
		if(num_pagina>=cant_paginas_procesos[idp]){//pedido invalido
			log_info(logger, "EL pedido de pagina %d del proceso %d es invalido",num_pagina,idp);
		}else{//el pedido es valido
			log_info(logger, "Pedido de pagina valido");
		//pido pagina a SWAP
			log_info(logger, "Solicitud de pagina a SWAP");
		void* mensaje;
		int tamanioMensaje = serializarLeerPagina(idp, num_pagina, &mensaje);
		enviar(socket_swap, mensaje, tamanioMensaje);
		free(mensaje);
		char* contenido_pagina=recibir_string_generico(socket_swap);
		log_info(logger,"se recibio la pagina, contenido: %s",contenido_pagina);
		//copiar en memoria la pagina
		log_info(logger, "Copia de la pagina %d en memoria",num_pagina);
		copiar_pagina_en_memoria(idp,num_pagina,contenido_pagina);
		//retomo el pedido de lectura
		log_info(logger, "Lectura de la pagina %d",num_pagina);
		char* contenido = leer_posicion_memoria(tabla_procesos[idp][num_pagina].marco * marco_size + offset, t);
		enviar_string(((t_cliente*)cpu)->socket,contenido);
		log_info(logger, "Contenido enviado: %s", contenido);
		free(contenido);
		free(contenido_pagina);
		}
	}
	pthread_mutex_unlock(&lock);
}

void escribir_pagina(int num_pagina, int offset, size_t t, char *buffer,void* cpu) { //testeado
	int idp=((t_cliente*)cpu)->proceso_activo;
	pthread_mutex_lock(&lock);
	int marco=obtener_marco(idp,num_pagina);
	if(marco!=-1){//esta en memoria
		log_info(logger, "Pagina %d, encontrada en memoria en el marco %d",num_pagina,marco);
		escribir_posicion_memoria(marco* marco_size + offset, t, buffer);
		tabla_procesos[idp][num_pagina].modificado=1; //modifique una pagina en memoria ->swap desactualizada
	}else{//no esta en memoria -> page fault
		log_info(logger, "Page Fault, no se encontró la pagina %d del proceso %d en memoria",num_pagina,idp);
		if(num_pagina>=cant_paginas_procesos[idp]){//pedido invalido
			log_info(logger, "EL pedido de pagina %d del proceso %d es invalido",num_pagina,idp);
		}else{ //el pedido es valido
			log_info(logger, "Pedido de pagina valido");
		//pido la pagina a swap
			log_info(logger, "Solicitud de pagina a SWAP");
		void* mensaje;
				int tamanioMensaje = serializarLeerPagina(idp, num_pagina, &mensaje);
				enviar(socket_swap, mensaje, tamanioMensaje);
				free(mensaje);
				char* contenido_pagina=recibir_string_generico(socket_swap);
		//copiar pagina en memoria
			log_info(logger, "Copia de la pagina %d del proceso %d en memoria",num_pagina,idp);
		copiar_pagina_en_memoria(idp,num_pagina,contenido_pagina);
		free(contenido_pagina);
		//retomo el pedido de escritura
		escribir_posicion_memoria(tabla_procesos[idp][num_pagina].marco* marco_size + offset, t, buffer);
			}
	}
	pthread_mutex_unlock(&lock);
	}


void copiar_pagina_en_memoria(int idp,int num_pagina,char* contenido_pagina){
	if (cant_paginas_asignadas(idp) < marco_x_proc) {
						int marco_libre = buscar_marco_libre();
							if (marco_libre != -1) {//hay un marco libre en memoria
							log_info(logger, "Se encontro el marco %d libre",marco_libre);
							escribir_posicion_memoria(marco_libre * marco_size, marco_size,	contenido_pagina);
							escribir_marco_en_TP(idp, num_pagina, marco_libre);
							marco_ocupado(marco_libre);
							if (tlb_habilitada){
								escribir_marco_en_tlb(idp,num_pagina,marco_libre);
							}
							}else {//memoria ocupada
							log_info(logger, "No se encontraron marcos libres en memoria, se reemplazara una pagina del proceso");
						//reemplazo local
						int marco = reemplazar_MP(idp, num_pagina);
						escribir_posicion_memoria(marco * marco_size, marco_size,contenido_pagina);
						escribir_marco_en_TP(idp, num_pagina, marco);
						if(tlb_habilitada){
							escribir_marco_en_tlb(idp,num_pagina,marco);
						}
							}
					}else {//ya tiene todos los marcos asignados
						log_info(logger, "El proceso tiene asignados todos los marcos, reemplazo local");
						int marco_destino = reemplazar_MP(idp, num_pagina);
						escribir_posicion_memoria(marco_destino * marco_size, marco_size,contenido_pagina);
						escribir_marco_en_TP(idp, num_pagina, marco_destino);
						if(tlb_habilitada){
							escribir_marco_en_tlb(idp,num_pagina,marco_destino);
						}
						log_info(logger, "Actualización de la tabla de paginas del proceso %d",idp);
					}
}



void crear_tabla_de_paginas(int idp, int paginas_requeridas) { //testeado
	log_info(logger, "Crear la tabla de paginas para el proceso %d",idp);
	tabla_paginas tabla_paginas;
	//inicializo
	int i;
	for (i = 0; i < paginas_requeridas; i++) {
		tabla_paginas.marco = -1;
		tabla_paginas.presencia = 0;
		tabla_paginas.bit_uso = 0;
		tabla_paginas.modificado = 0;
		tabla_procesos[idp][i] = tabla_paginas;
	}
	cant_paginas_procesos[idp] = paginas_requeridas;
	procesos_ocupados[idp] = 1;
}

int obtener_marco(int idp, int num_pagina) { //testeado
	int marco;
	if (tlb_habilitada) {
		log_info(logger, "TLB habilitada, buscar pagina");
		marco = obtener_marco_tlb(idp, num_pagina);
		if (marco == -1) { //si no esta en la tlb
			log_info(logger, "No se encontro la pagina en la TLB, busco en la tabla de paginas");
			marco = obtener_marco_tabla_paginas(idp, num_pagina);
			if (marco != -1) { //no estaba en la tlb pero si en la tabla de paginas(en MP)
				log_info(logger, "Se encontro la pagina en la tabla de paginas, actualizar TLB");
				escribir_marco_en_tlb(idp, num_pagina, marco);
			}
		}
		aumentar_uso_tlb(idp, num_pagina);
	} else { //si no esta habilitada la tlb
		log_info(logger, "TLB no habilitada, busco en la tabla de paginas");
		marco = obtener_marco_tabla_paginas(idp, num_pagina);
	}
	return marco;
}

int obtener_marco_tabla_paginas(int idp, int numero_pagina) { //testeado
	usleep(retardo * 1000);
	if (tabla_procesos[idp][numero_pagina].presencia == 1) {
		tabla_procesos[idp][numero_pagina].bit_uso = 1;
		return tabla_procesos[idp][numero_pagina].marco;
	} else {
		return -1;
	}

}

int obtener_marco_tlb(int idp, int num_pagina) { //testeado
	int i = 0;
	while (i < entradas_tlb) {
		if (tlb[i].idp == idp && tlb[i].pagina == num_pagina) {
			return tlb[i].marco;
		}
		i++;
	}

	return -1;
}

void escribir_marco_en_TP(int idp, int pagina, int marco) { //al guardar en MP devuelve el marco en que se guardo y se guarda en la tabla de paginas del proceso
	log_info(logger, "Actualizar tabla de paginas del proceso %d",idp);
	tabla_procesos[idp][pagina].marco = marco; //testeado
	tabla_procesos[idp][pagina].presencia = 1;
	tabla_procesos[idp][pagina].bit_uso = 1;
	tabla_procesos[idp][pagina].modificado = 0;
}

void escribir_marco_en_tlb(int idp, int num_pagina, int marco) {	//testeado
	if (tlb_habilitada){
	log_info(logger, "Nueva entrada en la TLB: pagina %d del proceso %d en el marco %d",num_pagina,idp,marco);
	int indice_libre = buscar_indice_libre_tlb();
	if (indice_libre != -1) { //si hay un indice libre
		log_info(logger, "Se encontro el indice %d libre",indice_libre);
		tlb[indice_libre].idp = idp;
		tlb[indice_libre].pagina = num_pagina;
		tlb[indice_libre].marco = marco;
		tlb[indice_libre].uso = 0;
	} else { //no hay indice libre ->reemplazo con LRU
		log_info(logger, "TLB llena, reemplazo con LRU");
		int indice_menos_accedido = buscar_indice_menos_accedido_tlb();
		tlb[indice_menos_accedido].idp = idp;
		tlb[indice_menos_accedido].pagina = num_pagina;
		tlb[indice_menos_accedido].marco = marco;
		tlb[indice_menos_accedido].uso = 0;
	}
}
}
void marco_ocupado(int num_marco) { 	//testeado
	marcos_libres[num_marco] = 1;
}
void marco_desocupado(int num_marco) { 	//testeado
	marcos_libres[num_marco] = 0;
}

void cambiar_proceso_activo(int idp, void* cliente) {
	log_info(logger, "Nuevo proceso activo: %d",idp); //controlo si la TLB esta habilitada en la funcion flush
	flush(((t_cliente*) cliente)->proceso_activo); //limpio lo que haya de ese proceso en la tlb
	((t_cliente*) cliente)->proceso_activo = idp;
}

void escribir_posicion_memoria(int posicion, size_t tamanio, char *buffer) {
	usleep(retardo * 1000);

	int i;
	for (i = 0; i < tamanio; i++)
		memoria[posicion + i] = buffer[i];

}

char* leer_posicion_memoria(int posicion, size_t tamanio) {
	usleep(retardo * 1000);
	int i;
	char * leido = malloc((tamanio + 1) * sizeof(char));
	for (i = 0; i < tamanio; i++)
		leido[i] = memoria[posicion + i];
	leido[tamanio] = '\0';
	return leido;
}

int buscar_indice_libre_tlb() { //testeado
	if(tlb_habilitada){
	int i;
	for (i = 0; i < entradas_tlb; i++) {
		if (tlb[i].idp == -1) {
			return i;
		}
	}
	return -1;
}
	return -1;
}

void aumentar_uso_tlb(int idp, int num_pagina) { //testeado
	if(tlb_habilitada){
	int i;
	for (i = 0; i < entradas_tlb; i++) {
		if (tlb[i].idp != idp && tlb[0].pagina != num_pagina) {
			tlb[i].uso += 1;
		} else {
			tlb[i].uso = 0;
		}
	}
}
}

int buscar_indice_menos_accedido_tlb() { //testeado
	if(tlb_habilitada){
	int indice, valormax = 0, i;
	for (i = 0; i < entradas_tlb; i++) {
		if (tlb[i].uso >= valormax) {
			valormax = tlb[i].uso;
			indice = i;
		}
	}
	return indice;
}
	return -1;
}

void flush(int idp) { //cuando cambia el proceso activo limpio el proceso viejo de la tlb
	if(tlb_habilitada){
	//log_info(logger, "Limpieza del proceso %d de la TLB por cambio de proceso activo", idp);
	int i = 0;
	for (i = 0; i < entradas_tlb; i++) {
		if (tlb[i].idp == idp) {
			tlb[i].idp = -1; //queda nuevamente como libre
		}
	}
}
}

//operaciones de consola
void modificar_retardo(int ret) { //testeado
	log_info(logger, "Comando modificar_retardo: se modifica el retardo de acceso a memoria a %d",ret);
	retardo = ret;
}

void dump_est_proceso(int idp, const char * nombreArchivo) { //generar reporte en pantalla y un archivo sobre la tabla de paginas del proceso
	log_info(logger, "Comando dump_est_proceso: generar un reporte sobre el contenido de la tabla de paginas del proceso %d",idp);
	int pag;
	int cantidad_paginas = cant_paginas_procesos[idp];
	FILE *archivo;
	if (strcmp(nombreArchivo, "tablas_de_paginas.txt"))
		archivo = fopen(nombreArchivo, "a");
	else
		archivo = fopen("tablas_de_paginas.txt", "a");
	if (!archivo) {
		printf("Error al abrir/crear el archivo! :(\n");

	} else
		fprintf(archivo, "Tabla de Paginas del Proceso: %d\n", idp);
	printf("Tabla de Paginas del Proceso: %d\n", idp);
	for (pag = 0; pag < cantidad_paginas; pag++) {
		printf(	"Pagina: %d, Marco: %d, Presencia: %d, Bit_uso: %d, Modificado: %d\n",
				pag, tabla_procesos[idp][pag].marco,
				tabla_procesos[idp][pag].presencia,
				tabla_procesos[idp][pag].bit_uso,
				tabla_procesos[idp][pag].modificado);
		if(archivo)
				fprintf(archivo,
				"Pagina: %d, Marco: %d, Presencia: %d, Bit_uso: %d, Modificado: %d\n",
				pag, tabla_procesos[idp][pag].marco,
				tabla_procesos[idp][pag].presencia,
				tabla_procesos[idp][pag].bit_uso,
				tabla_procesos[idp][pag].modificado);
	}
	printf("\n");
	if (archivo) {
		fprintf(archivo, "\n");
		fclose(archivo);
	}
}

void dump_est_gen() { //generar un reporte en pantalla y un archivo con las tablas de paginas de todos los procesos
	log_info(logger, "Comando dump_est_gen: generar un reporte sobre el contenido de las tablas de paginas de todos los procesos");
	FILE *archivo = fopen("tablas_de_paginas_todos_los_procesos.txt", "a");
	if (!archivo) {
		printf("Error al abrir/crear el archivo! :(\n");
	} else {
		fprintf(archivo, "Tablas de Paginas de todos los Procesos:\n\n");
		fclose(archivo);
	}
	printf("Tablas de Paginas de todos los Procesos:\n\n");
	int proceso;
	for (proceso = 0; proceso < 50; proceso++) {
		int proceso_existente = procesos_ocupados[proceso];
		if (proceso_existente) {
			dump_est_proceso(proceso,
					"tablas_de_paginas_todos_los_procesos.txt");
		}
	}
	fopen("tablas_de_paginas_todos_los_procesos.txt", "a");
	if (!archivo) {
		printf("Error al escribir finalizacion de archivo! :(\n");
	} else {
		fprintf(archivo,
				"------------------------------------------------------------------------------\n");
		fclose(archivo);
	}
}

void dump_cont_proceso(int idp, const char* nombreArchivo) { //testeado
//generar un reporte en pantalla y un archivo sobre los datos almacenados en memoria
//de ese proceso
	log_info(logger, "Comando dump_cont_proceso: generar un reporte sobre los datos almacenados en memoria del proceso %d",idp);
	int paginas_proceso=cant_paginas_procesos[idp];
	FILE *archivo;
		if (strcmp(nombreArchivo, "contenido_en_memoria.txt"))
			archivo = fopen(nombreArchivo, "a");
		else
			archivo = fopen("contenido_en_memoria.txt", "a");
		if (!archivo) {
			printf("Error al abrir/crear el archivo! :(\n");

		} else
			fprintf(archivo,"Contenido en Memoria Principal del proceso: %d\n",idp);
		printf("Contenido en Memoria Principal del proceso: %d\n",idp);

int i;
char* contenido;
for(i=0;i<paginas_proceso;i++){
	if(tabla_procesos[idp][i].presencia==1){
		contenido=leer_posicion_memoria(tabla_procesos[idp][i].marco*marco_size,marco_size);
		printf("Pagina: %d, contenido en memoria: %s\n",i,contenido);
		if(archivo)
			fprintf(archivo,"Pagina: %d, contenido en memoria: %s\n",i,contenido);
	}
}
printf("\n");
if (archivo) {
		fprintf(archivo, "\n");
		fclose(archivo);
	}
}

void dump_cont_gen() { //testeado
//generar un reporte en pantalla y un archivo sobre los datos almacenados en memoria
//de todos los procesos
	log_info(logger, "Comando dump_cont_gen: generar un reporte sobre los datos almacenados en memoria de todos los procesos");
	FILE *archivo = fopen("contenido_en_memoria_de_todos_los_procesos.txt", "a");
		if (!archivo) {
			printf("Error al abrir/crear el archivo! :(\n");
		} else {
			fprintf(archivo, "Contenido en Memoria de todos los procesos:\n\n");
			fclose(archivo);
		}
	printf("Contenido en Memoria de todos los procesos:\n\n");
	int proceso, proceso_existente;
		for (proceso = 0; proceso < 50; proceso++) {
			proceso_existente = procesos_ocupados[proceso];
			if (proceso_existente) {
				dump_cont_proceso(proceso, "contenido_en_memoria_de_todos_los_procesos.txt");
}
		}
	fopen("contenido_en_memoria_de_todos_los_procesos.txt", "a");
		if (!archivo) {
				printf("Error al escribir finalizacion de archivo! :(\n");
			} else {
				fprintf(archivo,
						"------------------------------------------------------------------------------\n");
				fclose(archivo);
			}
}

void flush_tlb() { //limpia completamente la tlb
	if(tlb_habilitada){
	log_info(logger, "Comando flush_tlb: limpia completamente la TLB");
	int i;
	for (i = 0; i < entradas_tlb; i++) {
		tlb[i].idp = -1;
	}
	}
}

void flush_memory(int idp) { //marca todas las paginas del proceso como modificadas
	log_info(logger, "Comando flush_memory: marcar todas las paginas del proceso como modificadas");
	int cant_paginas = cant_paginas_procesos[idp];
	int i;
	for (i = 0; i < cant_paginas; i++) {
		tabla_procesos[idp][i].modificado = 1;
	}
}

void terminar() {
	flagTerminar = 1;
	sem_post(&semTerminar);
}

int buscar_marco_libre() {
	int i;
	for (i = 0; i < marcos; i++) {
		if (marcos_libres[i] == 0) {
			return i;
		}
	}
	return -1;
}
void reconocer_comando(char *comando, char* param) {
	log_info(logger, "Reconocer comando %s %s",comando,param);
	pthread_mutex_lock(&lock);
	if (!strcmp(comando, "retardo")) {
		modificar_retardo(atoi(param));
	} else if (!strcmp(comando, "dump_est") && !strcmp(param, "gen")) {
		dump_est_gen();
	} else if (!strcmp(comando, "dump_est") && strcmp(param, "gen")) {
		dump_est_proceso(atoi(param), "tabla_paginas_proceso.txt");
	} else if (!strcmp(comando, "dump_cont") && !strcmp(param, "gen")) {
		dump_cont_gen();
	} else if (!strcmp(comando, "dump_cont") && strcmp(param, "gen")) {
		dump_cont_proceso(atoi(param),"contenido_en_memoria.txt");
	} else if (!strcmp(comando, "flush") && !strcmp(param, "tlb")) {
		flush_tlb();
	} else if (!strcmp(comando, "flush_memory")) {
		flush_memory(atoi(param));
	} else {
		printf("No existe el comando");
	}
	pthread_mutex_unlock(&lock);
}
int cant_paginas_asignadas(int idp) { //testeado
	int i;
	int contador = 0;
	for (i = 0; i < cant_paginas_procesos[idp]; i++) {
		if (tabla_procesos[idp][i].presencia == 1) {
			contador++;
		}
	}
	return contador;
}

int reemplazar_MP(int idp, int num_pagina) { //testeado
	int pagina_victima = buscar_pagina_victima(idp);
	log_info(logger, "La pagina a reemplazar: %d",pagina_victima);
	tabla_procesos[idp][pagina_victima].presencia = 0;
	sacar_pagina_de_tlb(idp,pagina_victima);
	int marco_destino = tabla_procesos[idp][pagina_victima].marco;
	log_info(logger, "El marco en que se copiara la nueva pagina: %d",marco_destino);
	if (tabla_procesos[idp][pagina_victima].modificado) {
			//enviar a swap que escriba la pagina victima (en swap quedo desactualizada)
			void* mensaje;
			log_info(logger,"pedido de escritura de la pagina en swap");
			char* pagina=leer_posicion_memoria(marco_destino*marco_size,marco_size);
			log_info(logger, "contenido: %s, tamanio: %d", pagina, string_length(pagina));
			int tamanioMensaje = serializarEscribirPagina(idp, pagina_victima,pagina,&mensaje);
			enviar(socket_swap, mensaje, tamanioMensaje);
	}
	return marco_destino;

}


void sacar_pagina_de_tlb(int idp,int pagina){
	if(tlb_habilitada){
	log_info(logger, "Sacar de la TLB la pagina: %d, del proceso %d",pagina,idp);
	int i;
	for(i=0;i<entradas_tlb;i++){
		if(tlb[i].idp==idp && tlb[i].pagina==pagina){
			tlb[i].idp=-1;
		}
	}
}
}

int buscar_pagina_victima(int idp) {
	//segun clock o clock modificado
	 int paginas_proceso = cant_paginas_procesos[idp];
		if (!strcmp(alg_reemplazo, "CLOCK_MODIFICADO")) { //clock modificado   testeado
			log_info(logger, "Buscar pagina victima segun algoritmo clock modificado");
			int i;
			int indice;
			int iteracion;
		for(iteracion=0;iteracion<2;iteracion++){
			for(i=0;i<paginas_proceso;i++){
				indice=punteros_clock[idp];
				if(tabla_procesos[idp][indice].bit_uso==0 && tabla_procesos[idp][indice].modificado==0 && tabla_procesos[idp][indice].presencia==1){
					punteros_clock[idp]+=1;
					if(punteros_clock[idp]==paginas_proceso){
						punteros_clock[idp]=0;
					}
					return indice;
				}else{
					punteros_clock[idp] +=1;
					if(punteros_clock[idp]==paginas_proceso){
						punteros_clock[idp]=0;
					}
				}
			}
			//busco U=0, M=1
			for(i=0;i<paginas_proceso;i++){
				indice=punteros_clock[idp];
				if(tabla_procesos[idp][indice].bit_uso==0 && tabla_procesos[idp][indice].modificado==1 && tabla_procesos[idp][indice].presencia==1){
					punteros_clock[idp]+=1;
					if(punteros_clock[idp]==paginas_proceso){
						punteros_clock[idp]=0;
					}
					return indice;
				}else{
					tabla_procesos[idp][indice].bit_uso=0;
					punteros_clock[idp]+=1;
					if(punteros_clock[idp]==paginas_proceso){
						punteros_clock[idp]=0;
					}
				}
			}
		}//termina el ciclo

	} else if (!strcmp(alg_reemplazo, "CLOCK")) { //clock	testeado
		log_info(logger, "Buscar pagina victima segun algoritmo clock");
		int i;
		for (i = 0; i <= paginas_proceso; i++) {
			i = punteros_clock[idp];
			if (tabla_procesos[idp][i].bit_uso == 0
					&& tabla_procesos[idp][i].presencia == 1) {
				punteros_clock[idp] = i + 1;
				if(punteros_clock[idp]==paginas_proceso){
					punteros_clock[idp]=0;
				}
				return i; //devuelve la pagina(indice) victima
			} else {
				tabla_procesos[idp][i].bit_uso = 0;
				punteros_clock[idp] += 1;
				if (i == paginas_proceso - 1) {
					punteros_clock[idp] = 0;
				}
			}
		}
	}
	return -1;
}



