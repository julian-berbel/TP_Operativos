#include "nucleo.h"

void cancelar(int socket){

}

int buscarSocketConsola(int pid){
	_Bool compararPid(t_consola* consola){
		return consola->pid == pid;
	}

	t_consola* consola = list_find(consolas, (void*)compararPid);
	return consola->socketConsola;
}

void imprimir(int pid, char* mensaje){
	int socket_consola = buscarSocketConsola(pid);

	void* mensajeSerializado;
	int tamanio = serializarImprimir(mensaje, &mensajeSerializado);
	enviar(socket_consola, mensajeSerializado, tamanio);

	free(mensajeSerializado);
}

void quantumTerminado(t_PCB* pcbActualizado){//????
	/* buscar por pid y remover pcb desactualizado de colaPCBExec y destruirlo. (referirse a list_remove_and_destroy_by_condition de la commons)
	 meter pcbActualizado en la cola que corresponda.
	*/
	printf("Quantum Terminado del PCB: \n");
	imprimirPCB(pcbActualizado);
	pcb_destroy(pcbActualizado);
}

void nuevoPrograma(int socket_consola){
	char* programa = recibir_string_generico(socket_consola);
	static int pid = 0;
	int paginasRequeridas = ceil((double) string_length(programa) / (double) tamanioDePagina);

	void* mensaje;
	int tamanio = serializarInicializar(pid, paginasRequeridas, programa, &mensaje);
	enviar(socket_umc, mensaje, tamanio);

	// Manejo de error: No hay espacio para el programa

	t_consola* consola = malloc(sizeof(t_consola));
	consola->pid = pid;
	consola->socketConsola = socket_consola;
	list_add(consolas, consola);

	t_PCB* pcb = crearPCB(programa, pid);
	queue_push(colaPCBReady, pcb);

	pid++;
}

int mayorSocket(int mayor){
	int i;
	t_consola* consola;

	for(i = 0; i < consolas->elements_count; i++){
		consola = (t_consola*) list_get(consolas, i);
		if(consola->socketConsola > mayor) mayor = consola->socketConsola;
	}

	return mayor;
}

void destruirConsola(t_consola* consola){
	close(consola->socketConsola);
	free(consola);
}

void threadReceptorYEscuchaConsolas(void* param){
	fd_set* bagDeSockets = malloc(sizeof(fd_set));

	int i, conexionRecibida, socketServer = *((int*)param), n = mayorSocket(socketServer) + 1;

	t_consola* consola;

	void* mensaje;

	while(!flagTerminar){
		FD_ZERO(bagDeSockets);
		FD_SET(socketServer, bagDeSockets);

		for(i = 0; i < consolas->elements_count; i++){
			consola = (t_consola*) list_get(consolas, i);
			FD_SET(consola->socketConsola, bagDeSockets);
		}

		select(n, bagDeSockets, NULL, NULL, NULL);

		if(flagTerminar) break;

		if(FD_ISSET(socketServer, bagDeSockets)){
			conexionRecibida = recibirConexion(socketServer);
			printf("Conectado a una consola!\n");
			nuevoPrograma(conexionRecibida);
			n = mayorSocket(socketServer) + 1;
		}else{
			for(i = 0; i < consolas->elements_count; i++){
				consola = list_get(consolas, i);
				if (FD_ISSET(consola->socketConsola, bagDeSockets)) {
					mensaje = recibir(consola->socketConsola);
					if(mensaje){
						procesarMensaje(mensaje, consola);
					}else{
						printf("Consola desconectada!\n");
						list_remove_and_destroy_element(consolas, i,(void*) destruirConsola);
					}
					free(mensaje);
				}
			}
		}
	}
	free(bagDeSockets);
	close(socketServer);
}

void threadEscuchaCPU(void* param){
	int socket = *((int*) param);
	free(param);

	void* mensaje = NULL;

	printf("Conectado a un CPU\n");

	while(!flagTerminar){
		mensaje = recibir(socket);
		procesarMensaje(mensaje, NULL);
	}

	close(socket);
}

void crearThreadDeCPU(int conexionRecibida){
	pthread_t* hiloEscuchaCPU = malloc(sizeof(pthread_t));

	t_cpu* cpu = malloc(sizeof(t_cpu));
	cpu->socketCPU = conexionRecibida;
	cpu->hiloCPU = hiloEscuchaCPU;

	list_add(cpus, (void*) cpu);

	int* socket = malloc(sizeof(int));
	*socket = conexionRecibida;

	pthread_create(hiloEscuchaCPU, NULL,(void*) threadEscuchaCPU,(void*) socket);
}

void threadReceptorCPUs(void* param){
	int socketServer = *((int*)param);

	int conexionRecibida = recibirConexion(socketServer);

	while(!flagTerminar){
		crearThreadDeCPU(conexionRecibida);
		conexionRecibida = recibirConexion(socketServer);
	}

	close(socketServer);
}

void terminar(int n){
	if(n == SIGINT){
		flagTerminar = 1;
		sem_post(&semTerminar);
	}
}

void bloquearConsola(t_consola* consola){
	shutdown(consola->socketConsola, 0);
}

void destruirCPU(t_cpu* cpu){
	shutdown(cpu->socketCPU, 0);
	pthread_join(*cpu->hiloCPU, NULL);
	free(cpu->hiloCPU);
	free(cpu);
}

int main(){

	abrirConfiguracion();

	sem_init(&semTerminar, 0, 0);

	signal(SIGINT, terminar);

	consolas = list_create();
	cpus = list_create();

	log_info(logger, "Inicia proceso Núcleo");

	socket_umc = crear_socket_cliente(ipUMC, puertoUMC);
	log_info(logger_pantalla, "Nucleo y UMC conectados");

	socket_consolas = crear_socket_servidor(ipNucleo, puertoNucleo);

	pthread_create(&hiloConsolas, NULL,(void*) threadReceptorYEscuchaConsolas, &socket_consolas);

	socket_cpus = crear_socket_servidor(ipNucleo, puertoCPU);

	pthread_create(&hiloReceptorCPUs, NULL,(void*) threadReceptorCPUs, &socket_cpus);

	sem_wait(&semTerminar);

	cerrar_todo();

	return 0;
}

void abrirConfiguracion(){
	configuracionNucleo = config_create(RUTA_CONFIG);
	ipNucleo = config_get_string_value(configuracionNucleo, "IP_NUCLEO");
	puertoNucleo = config_get_string_value(configuracionNucleo, "PUERTO_PROG");
	puertoCPU = config_get_string_value(configuracionNucleo, "PUERTO_CPU");
	quantum = config_get_int_value(configuracionNucleo, "QUANTUM");
	quantum_sleep = config_get_int_value(configuracionNucleo, "QUANTUM_SLEEP");
	io_id = config_get_array_value(configuracionNucleo, "IO_ID");
	io_sleep = config_get_array_value(configuracionNucleo, "IO_SLEEP");
	sem_id = config_get_array_value(configuracionNucleo, "SEM_ID");
	sem_init1 = config_get_array_value(configuracionNucleo, "SEM_INIT");
	shared_vars = config_get_array_value(configuracionNucleo, "SHARED_VARS");
	logger = log_create(RUTA_LOG, "Nucleo", false, LOG_LEVEL_INFO);
	logger_pantalla = log_create(RUTA_LOG, "Nucleo", true, LOG_LEVEL_INFO);
	puertoUMC = config_get_string_value(configuracionNucleo, "PUERTO_UMC");
	ipUMC = config_get_string_value(configuracionNucleo, "IP_UMC");

	printf("%s\n", ipNucleo);
	printf("%s\n", puertoNucleo);
	printf("%s\n", puertoCPU);
	printf("%d\n", quantum);
	printf("%d\n", quantum_sleep);
	printf("%s\n", io_id[0]);
	printf("%s\n", ipUMC);
	printf("%s\n", puertoUMC);

}

void cerrar_todo(){
	shutdown(socket_consolas, 0);
	shutdown(socket_cpus, 0);
	shutdown(socket_umc, 0);

	list_iterate(consolas, (void*) bloquearConsola);
	pthread_join(hiloConsolas, NULL);
	list_destroy_and_destroy_elements(consolas, (void*) destruirConsola);

	list_destroy_and_destroy_elements(cpus,(void*) destruirCPU);

	pthread_join(hiloReceptorCPUs, NULL);

	sem_destroy(&semTerminar);

	close(socket_umc);

	int i;

	int largo_array = sizeof(io_id);
	for(i=0; i<largo_array; ++i)
		free(io_id[i]);
	free(io_id);

	largo_array = sizeof(io_sleep);
	for(i=0; i<largo_array; ++i)
		free(io_sleep[i]);
	free(io_sleep);

	largo_array = sizeof(sem_id);
	for(i=0; i<largo_array; ++i)
		free(sem_id[i]);
	free(sem_id);

	largo_array = sizeof(sem_init1);
	for(i=0; i<largo_array; ++i)
		free(sem_init1[i]);
	free(sem_init1);

	largo_array = sizeof(shared_vars);
	for(i=0; i<largo_array; ++i)
		free(shared_vars[i]);
	free(shared_vars);

	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracionNucleo);
}
