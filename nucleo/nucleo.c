#include "nucleo.h"

void threadReceptorConsolas(void* param){
	int socketServer = *((int*)param);
	int* conexionRecibida = malloc(sizeof(int));

	*conexionRecibida = recibirConexion(socketServer);

	while(!flagTerminar){
		printf("Conectado a una Consola\n");
		list_add(consolas, (void*) conexionRecibida);
		conexionRecibida = malloc(sizeof(int));
		*conexionRecibida = recibirConexion(socketServer);
	}
	free(conexionRecibida);

	close(socketServer);

}

void threadCPU(void* param){
	int socket = *((int*) param);
	free(param);

	void* mensaje = NULL;

	printf("Conectado a un CPU\n");

	while(!flagTerminar){
		mensaje = recibir(socket);
		//procesarMensaje(mensaje); // hay que definir el protocolo Nucleo <--> CPU
		free(mensaje);
	}

	close(socket);
}

void crearThreadDeCPU(int conexionRecibida){
	pthread_t* hiloCPU = malloc(sizeof(pthread_t));

	t_cpu* cpu = malloc(sizeof(t_cpu));
	cpu->socketCPU = conexionRecibida;
	cpu->hiloCPU = hiloCPU;

	list_add(cpus, (void*) cpu);

	int* socket = malloc(sizeof(int));
	*socket = conexionRecibida;

	pthread_create(hiloCPU, NULL,(void*) threadCPU,(void*) socket);
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

void cerrarSocket(int* socket){
	close(*socket);
	free(socket);
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

	pthread_create(&thrdReceptorConsolas, NULL,(void*) threadReceptorConsolas, &socket_consolas);

	socket_cpus = crear_socket_servidor(ipNucleo, puertoCPU);

	pthread_create(&thrdReceptorCPUs, NULL,(void*) threadReceptorCPUs, &socket_cpus);

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

	list_destroy_and_destroy_elements(consolas,(void*) cerrarSocket);

	list_destroy_and_destroy_elements(cpus,(void*) destruirCPU);

	pthread_join(thrdReceptorConsolas, NULL);

	pthread_join(thrdReceptorCPUs, NULL);

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
