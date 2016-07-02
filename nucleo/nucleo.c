#include "nucleo.h"

int buscarSocketConsola(int pid){
	log_info(logger, "Buscar Socket de Consola: pid: %d", pid);
	_Bool compararPid(t_consola* consola){
		return consola->elemento->pcb->pid == pid;
	}

	t_consola* consola = list_find(consolas, (void*)compararPid);
	return consola ? consola->socketConsola : -1;
}

void imprimir(char* mensaje, void* cpu){
	if(((t_cpu*) cpu)->elemento->estado == EXIT) return;
	log_info(logger, "Imprimir: mensaje: %s", mensaje);
	int socket_consola = buscarSocketConsola(((t_cpu*)cpu)->elemento->pcb->pid);
	void* mensajeSerializado;
	int tamanio = serializarImprimir(mensaje, &mensajeSerializado);
	enviar(socket_consola, mensajeSerializado, tamanio);

	free(mensajeSerializado);
}

t_elemento_cola* actualizarPCB(t_list* lista, t_PCB* pcbActualizado){
	log_info(logger, "Actualizar PCB: pid: %d", pcbActualizado->pid);
	_Bool compararPid(t_elemento_cola* elemento){
		return elemento->pcb->pid == pcbActualizado->pid;
	}

	t_elemento_cola* elemento = list_find(lista, (void*) compararPid);
	pcb_destroy(elemento->pcb);
	elemento->pcb = pcbActualizado;
	return elemento;
}

t_elemento_cola* desalojar(t_cpu* cpu){
	log_info(logger, "Desalojar CPU: pid: %d", cpu->elemento->pcb->pid);
	void* mensaje;
	int tamanioMensaje;

	tamanioMensaje = serializarDesalojar(&mensaje);
	enviar(cpu->socketCPU, mensaje, tamanioMensaje);
	free(mensaje);

	mensaje = recibir(cpu->socketCPU);
	t_PCB* pcbActualizado = deserializarPCB(mensaje);
	free(mensaje);
	t_elemento_cola* elemento = actualizarPCB(colaPCBExec->elements, pcbActualizado);

	cpu->elemento = NULL;
	return elemento;
}

void quantumTerminado(void* cpu){
	if(((t_cpu*) cpu)->elemento->estado == EXIT) return;
	log_info(logger, "Quantum Terminado: pid: %d", ((t_cpu*)cpu)->elemento->pcb->pid);
	void* mensaje;
	int tamanioMensaje;
	pthread_mutex_lock(&mutexColaReady);
	if(!hayCPUsLibres() && colaPCBReady->elements->elements_count){
		pthread_mutex_unlock(&mutexColaReady);

		t_elemento_cola* elemento = desalojar(cpu);

		elemento->estado = READY;
		sem_post(&moverPCBs);
	}else{
		pthread_mutex_unlock(&mutexColaReady);
		tamanioMensaje = serializarContinuarEjecucion(&mensaje);
		enviar(((t_cpu*)cpu)->socketCPU, mensaje, tamanioMensaje);
		free(mensaje);
	}
}

void nuevoPrograma(int socket_consola){
	log_info(logger, "Procesando nuevo programa: socket consola: %d", socket_consola);
	char* programa = recibir_string_generico(socket_consola);
	static int pid = 0;
	int paginasRequeridas = ceil((double) string_length(programa) / (double) tamanioDePagina) + stack_size;

	void* mensaje;
	int tamanio = serializarInicializar(pid, paginasRequeridas, programa, &mensaje);
	enviar(socket_umc, mensaje, tamanio);
	free(mensaje);
	char* respuesta = recibir_string_generico(socket_umc);

	if(!strcmp(respuesta, "OK")){

		t_consola* consola = malloc(sizeof(t_consola));
		consola->elemento = malloc(sizeof(t_elemento_cola));

		int aux;
		pthread_mutex_lock(&mutexPID);
		aux = pid;
		pid++;
		pthread_mutex_unlock(&mutexPID);

		consola->socketConsola = socket_consola;
		list_add(consolas, consola);

		consola->elemento->pcb = crearPCB(programa, aux);
		consola->elemento->pcb->cantidadPaginas = paginasRequeridas;

		consola->elemento->estado = EXEC;

		pthread_mutex_lock(&mutexColaReady);
		queue_push(colaPCBReady, consola->elemento);
		pthread_mutex_unlock(&mutexColaReady);
		sem_post(&moverPCBs);
	}else{
		tamanio = serializarImprimir("Espacio insuficiente!", &mensaje);
		enviar(socket_consola, mensaje, tamanio);
		free(mensaje);

		tamanio = serializarTerminar(&mensaje);
		enviar(socket_consola, mensaje, tamanio);
		free(mensaje);
	}
}

int mayorSocket(int mayor){
	log_info(logger, "Buscando mayor socket");
	int i;
	t_consola* consola;

	for(i = 0; i < consolas->elements_count; i++){
		consola = (t_consola*) list_get(consolas, i);
		if(consola->socketConsola > mayor) mayor = consola->socketConsola;
	}

	return mayor;
}

void leerCambios(){
	log_info(logger, "Leyendo cambios en archivo de config");
	t_config* configAux = config_create(RUTA_CONFIG);
	quantum = config_get_int_value(configAux, "QUANTUM");
	quantum_sleep = config_get_int_value(configAux, "QUANTUM_SLEEP");

	config_destroy(configAux);
}

void destruirConsola(t_consola* consola){
	log_info(logger, "Destruyendo consola: socket consola: %d", consola->socketConsola);
	close(consola->socketConsola);
	consola->elemento->estado = EXIT;
	sem_post(&moverPCBs);
	free(consola);
}

void threadReceptorYEscuchaConsolas(){
	log_info(logger, "Iniciado Thread Receptor y Escucha Consolas");
	fd_set* bagDeSockets = malloc(sizeof(fd_set));
	socket_consolas = crear_socket_servidor(ipNucleo, puertoNucleo);

	int i, conexionRecibida, descriptor_inotify = inotify_init(), n;
	inotify_add_watch(descriptor_inotify, RUTA_CONFIG, IN_MODIFY);

	n = (socket_consolas> descriptor_inotify) ? socket_consolas:descriptor_inotify;
	n = mayorSocket(n) + 1;

	t_consola* consola;

	void* mensaje;

	while(!flagTerminar){
		FD_ZERO(bagDeSockets);
		FD_SET(socket_consolas, bagDeSockets);
		FD_SET(descriptor_inotify, bagDeSockets);

		for(i = 0; i < consolas->elements_count; i++){
			consola = (t_consola*) list_get(consolas, i);
			FD_SET(consola->socketConsola, bagDeSockets);
		}

		select(n, bagDeSockets, NULL, NULL, NULL);

		if(flagTerminar) break;

		if(FD_ISSET(socket_consolas, bagDeSockets)){
			conexionRecibida = recibirConexion(socket_consolas);
			nuevoPrograma(conexionRecibida);
			n = mayorSocket(n) + 1;
		}else if(FD_ISSET(descriptor_inotify, bagDeSockets)){
			mensaje = malloc(sizeof(struct inotify_event));
			read(descriptor_inotify, mensaje, sizeof(struct inotify_event));
			read(descriptor_inotify, mensaje, sizeof(struct inotify_event)); //odio esto con furor, pero sino corre esta parte del codigo 2 veces
			usleep(10000);
			leerCambios();
			free(mensaje);
		}else{
			for(i = 0; i < consolas->elements_count; i++){
				consola = list_get(consolas, i);
				if (FD_ISSET(consola->socketConsola, bagDeSockets)){
					printf("Destruyendo una consola\n");
					list_remove_and_destroy_element(consolas, i,(void*) destruirConsola);
				}
			}
		}
	}
	free(bagDeSockets);
	close(socket_consolas);
}

void cerrarCPU(void* cpu, void* ultimoMensaje){
	_Bool buscarCPU(void* elementoDeLaLista){
			return elementoDeLaLista == cpu;
	}

	pthread_mutex_lock(&mutexCPUs);
	list_remove_by_condition(cpus, (void*) buscarCPU(cpu));
	pthread_mutex_unlock(&mutexCPUs);

	log_info(logger, "Destruyendo CPU: thread: %d", ((t_cpu*)cpu)->hiloCPU);

	if(ultimoMensaje){
		if(((t_cpu*)cpu)->elemento){
			t_elemento_cola* elemento = desalojar(cpu);
			elemento->estado = READY;
		}

		void* mensaje;
		int tamanioMensaje = serializarTerminar(&mensaje);
		enviar(((t_cpu*)cpu)->socketCPU, mensaje, tamanioMensaje);
		free(mensaje);

		shutdown(((t_cpu*)cpu)->socketCPU, 0);
	}else{
		// imprimir mensaje de error en la consola?
		((t_cpu*)cpu)->elemento->estado = EXIT;
	}
	sem_post(&moverPCBs);
	free(((t_cpu*)cpu)->hiloCPU);
	free(cpu);
}

void programaTerminado(void* cpu){
	((t_cpu*)cpu)->elemento->estado = EXIT;
	((t_cpu*)cpu)->elemento = NULL;
	sem_post(&moverPCBs);
}

void threadEscuchaCPU(t_cpu* cpu){
	_Bool buscarCPU(void* elementoDeLaLista){
		return elementoDeLaLista == cpu;
	}

	log_info(logger, "Iniciado thread de CPU: thread: %d", cpu->hiloCPU);
	int socket = cpu->socketCPU;

	enviar(socket, &stack_size, sizeof(int));

	void* mensaje;

	printf("Conectado a un CPU\n");

	while(!flagTerminar){
		mensaje = recibir(socket);
		if(!mensaje) break;
		procesarMensaje(mensaje, cpu);
	}

	if(!flagTerminar && !mensaje) cerrarCPU(cpu, mensaje);//CPU desconectado a la fuerza

	close(socket);
}

void crearThreadDeCPU(int conexionRecibida){
	log_info(logger, "Creando Thread de CPU");
	pthread_t* hiloEscuchaCPU = malloc(sizeof(pthread_t));

	t_cpu* cpu = malloc(sizeof(t_cpu));
	cpu->socketCPU = conexionRecibida;
	cpu->hiloCPU = hiloEscuchaCPU;
	cpu->elemento = NULL;

	pthread_mutex_lock(&mutexCPUs);
	list_add(cpus, (void*) cpu);
	pthread_mutex_unlock(&mutexCPUs);

	pthread_create(hiloEscuchaCPU, NULL,(void*) threadEscuchaCPU, cpu);
}

void threadReceptorCPUs(void* param){
	log_info(logger, "Iniciado Receptor de CPUs");
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
		log_info(logger, "Recibida senial SIGINT");
		flagTerminar = 1;
		sem_post(&semTerminar);
	}
}

void bloquearConsola(t_consola* consola){
	log_info(logger, "Bloqueando consola");
	shutdown(consola->socketConsola, 0);
}

void destruirCPU(t_cpu* cpu){
	log_info(logger, "Destruyendo CPU: thread: %d", cpu->hiloCPU);
	shutdown(cpu->socketCPU, 0);
	pthread_join(*cpu->hiloCPU, NULL);
	free(cpu->hiloCPU);
	free(cpu);
}

int main(){

	abrirConfiguracion();

	log_info(logger, "Inicia proceso Núcleo");

	colaPCBExec = queue_create();
	colaPCBReady = queue_create();
	listaPCBBlock = list_create();
	listaPCBExit = list_create();

	int i, tamanio;
	tamanio = tamanioArray(io_id);
	dispositivos = malloc(sizeof(t_dispositivo) * tamanio);
	for(i = 0; i < tamanio; i++){
		pthread_t hiloDispositivo;
		dispositivos[i].hiloDispositivo = hiloDispositivo;
		dispositivos[i].cola_dispositivo = queue_create();
		sem_t semaforoDispositivo;
		sem_init(&semaforoDispositivo, 0, 0);
		dispositivos[i].semaforoDispositivo = semaforoDispositivo;
		dispositivos[i].retardo = atoi(io_sleep[i]);
		pthread_create(&hiloDispositivo, NULL,(void*) threadDispositivo,(void*) &dispositivos[i]);
	}

	tamanio = tamanioArray(sem_id);
	semaforosGlobales = malloc(sizeof(t_semaforo) * tamanio);
	for(i = 0; i < tamanio; i++){
		char *ptr;
		semaforosGlobales[i].valor = strtol(sem_inits[i], &ptr, 10);
		semaforosGlobales[i].cola_bloqueados = queue_create();
	}

	tamanio = tamanioArray(shared_vars);
	variablesGlobales = malloc(sizeof(int) * tamanio);
	for(i = 0; i < tamanio; i++) variablesGlobales[i] = 0;

	sem_init(&moverPCBs, 0, 0);
	sem_init(&semTerminar, 0, 0);

	signal(SIGINT, terminar);

	consolas = list_create();
	cpus = list_create();

	socket_umc = crear_socket_cliente(ipUMC, puertoUMC);
	log_info(logger_pantalla, "Nucleo y UMC conectados");
	void* respuesta = recibir(socket_umc);
	tamanioDePagina = *(int*)respuesta;

	pthread_create(&hiloConsolas, NULL,(void*) threadReceptorYEscuchaConsolas, NULL);

	socket_cpus = crear_socket_servidor(ipNucleo, puertoCPU);

	pthread_create(&hiloReceptorCPUs, NULL,(void*) threadReceptorCPUs, &socket_cpus);

	pthread_create(&hiloPlanificador, NULL,(void*) threadPlanificador, NULL);

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
	sem_inits = config_get_array_value(configuracionNucleo, "SEM_INIT");
	shared_vars = config_get_array_value(configuracionNucleo, "SHARED_VARS");
	stack_size = config_get_int_value(configuracionNucleo, "STACK_SIZE");
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
	log_info(logger, "Comenzando cierre");
	shutdown(socket_consolas, 0);
	shutdown(socket_cpus, 0);
	shutdown(socket_umc, 0);
	close(descriptor_inotify);

	list_iterate(consolas, (void*) bloquearConsola);
	pthread_join(hiloConsolas, NULL);
	list_destroy_and_destroy_elements(consolas, (void*) destruirConsola);

	list_destroy_and_destroy_elements(cpus,(void*) destruirCPU);

	pthread_join(hiloReceptorCPUs, NULL);

	sem_destroy(&semTerminar);

	close(socket_umc);

	int i;
	int largo_array = tamanioArray(io_id);
	for(i = 0; i < largo_array; i++){
		pthread_join(dispositivos[i].hiloDispositivo, NULL);
		sem_destroy(&dispositivos[i].semaforoDispositivo);
		queue_destroy_and_destroy_elements(dispositivos[i].cola_dispositivo,free);

		free(io_id[i]);
		free(io_sleep[i]);
	}
	free(dispositivos);

	free(io_id);

	free(io_sleep);

	largo_array = tamanioArray(sem_id);
	for(i = 0; i < largo_array; i++){
		queue_destroy(semaforosGlobales[i].cola_bloqueados);
		free(sem_id[i]);
		free(sem_inits[i]);
	}
	free(semaforosGlobales);

	free(sem_id);

	free(sem_inits);

	largo_array = tamanioArray(shared_vars);
	for(i = 0; i < largo_array; i++) free(shared_vars[i]);
	free(shared_vars);

	log_destroy(logger);
	log_destroy(logger_pantalla);
	config_destroy(configuracionNucleo);
}

int indiceEnArray(char** array, char* elemento){
	int i = 0;
	while(array[i] && strcmp(array[i], elemento)) i++;

	return array[i] ? i:-1;
}

int tamanioArray(char** array){
	int i = 0;
	while(array[i]) i++;
	return i;
}

void obtener_valor(char* identificador, void* cpu){
	log_info(logger, "Obtener Valor: id: %s", identificador);
	int indice = indiceEnArray(shared_vars, identificador);
	int valor;
	pthread_mutex_lock(&mutexVariablesGlobales);
	valor = variablesGlobales[indice];
	pthread_mutex_unlock(&mutexVariablesGlobales);
	log_info(logger, "Valor obtenido: %d", valor);
	enviar(((t_cpu*) cpu)->socketCPU, &valor, sizeof(int));
}

void grabar_valor(char* identificador, int valorAGrabar){
	log_info(logger, "Grabar Valor: id: %s, valor: %d", identificador, valorAGrabar);
	int indice = indiceEnArray(shared_vars, identificador);
	pthread_mutex_lock(&mutexVariablesGlobales);
	variablesGlobales[indice] = valorAGrabar;
	pthread_mutex_unlock(&mutexVariablesGlobales);
}

void esperar(char* identificador, void* cpu){
	log_info(logger, "Esperar: id: %s", identificador);
	int indice = indiceEnArray(sem_id, identificador);
	log_info(logger, "Indice encontrado: %d", indice);
	log_info(logger, "Valor en el indice: %d", semaforosGlobales[indice].valor);
	pthread_mutex_lock(&mutexSemaforosGlobales);
	if(semaforosGlobales[indice].valor){
		semaforosGlobales[indice].valor--;
		pthread_mutex_unlock(&mutexSemaforosGlobales);
		enviar_string(((t_cpu*)cpu)->socketCPU, "dale para adelante!");
	}else{
		enviar_string(((t_cpu*)cpu)->socketCPU, "bloqueate!");
		t_elemento_cola* elemento = desalojar(cpu);

		queue_push(semaforosGlobales[indice].cola_bloqueados, elemento);
		pthread_mutex_unlock(&mutexSemaforosGlobales);

		elemento->estado = BLOCK;
		sem_post(&moverPCBs);
	}
}

void avisar(char* identificador){
	log_info(logger, "Avisar: id: %s", identificador);
	int indice = indiceEnArray(sem_id, identificador);
	pthread_mutex_lock(&mutexSemaforosGlobales);
	if(queue_peek(semaforosGlobales[indice].cola_bloqueados)){
		t_elemento_cola* elemento = queue_pop(semaforosGlobales[indice].cola_bloqueados);
		pthread_mutex_unlock(&mutexSemaforosGlobales);
		elemento->estado = READY;
		sem_post(&moverPCBs);
	}else{
		semaforosGlobales[indice].valor++;
		pthread_mutex_unlock(&mutexSemaforosGlobales);
	}
}

void entradaSalida(char* identificador, int operaciones, void* cpu){
	log_info(logger, "Entrada/Salida: id: %s, operaciones: %d", identificador, operaciones);
    t_pedido* pedido = malloc(sizeof(t_pedido));
    pedido->cantidadDeOperaciones = operaciones;

    t_elemento_cola* elemento = desalojar(cpu);

    pedido->elemento = elemento;
    elemento->estado = BLOCK;
    int indice = indiceEnArray(io_id, identificador);
    queue_push(dispositivos[indice].cola_dispositivo, (void*)pedido);
    sem_post(&dispositivos[indice].semaforoDispositivo);
    sem_post(&moverPCBs);
}

void threadDispositivo(t_dispositivo* dispositivo){
	//log_info(logger, "Iniciado Thread de dispositivo: thread: %d", dispositivo->hiloDispositivo);
    while(!flagTerminar){
        sem_wait(&dispositivo->semaforoDispositivo);
        if(flagTerminar) break;
        t_pedido* pedido = queue_pop(dispositivo->cola_dispositivo);
        if(!pedido->elemento) break;
        usleep(dispositivo->retardo * 1000 * pedido->cantidadDeOperaciones);
        if(!pedido->elemento) break;
        pedido->elemento->estado = READY;
        sem_post(&moverPCBs);
    }
}

void threadPlanificador(){
	log_info(logger, "Iniciado thread Planificador");
	int pidABuscar;
	_Bool compararPid(t_elemento_cola* elemento){
			return elemento->pcb->pid == pidABuscar;
	}

	int i;

	t_elemento_cola* elemento;

	t_cpu* cpu;

	while(!flagTerminar){
		sem_wait(&moverPCBs);
		//cola exec
		for(i = 0; i < colaPCBExec->elements->elements_count; i++){
			elemento = list_get(colaPCBExec->elements, i);
			if(elemento->estado == BLOCK){
				pidABuscar = elemento->pcb->pid;
				list_remove_by_condition(colaPCBExec->elements, (void*)compararPid);
				list_add(listaPCBBlock, elemento);
				i--;
			}else if(elemento->estado == READY){
				pidABuscar = elemento->pcb->pid;
				list_remove_by_condition(colaPCBExec->elements, (void*)compararPid);
				pthread_mutex_lock(&mutexColaReady);
				queue_push(colaPCBReady, elemento);
				pthread_mutex_unlock(&mutexColaReady);
				elemento->estado = EXEC;
				i--;
			}else if(elemento->estado == EXIT){
				pidABuscar = elemento->pcb->pid;
				list_remove_by_condition(colaPCBExec->elements, (void*)compararPid);
				list_add(listaPCBExit, elemento);
				i--;
			}
		}

		//lista block
		for(i = 0; i < listaPCBBlock->elements_count; i++){
			elemento = list_get(listaPCBBlock, i);
			if(elemento->estado == READY){
				pidABuscar = elemento->pcb->pid;
				list_remove_by_condition(listaPCBBlock, (void*)compararPid);
				pthread_mutex_lock(&mutexColaReady);
				queue_push(colaPCBReady, elemento);
				pthread_mutex_unlock(&mutexColaReady);
				elemento->estado = EXEC;
				i--;
			}else if(elemento->estado == EXIT){
				pidABuscar = elemento->pcb->pid;
				list_remove_by_condition(listaPCBBlock, (void*)compararPid);
				int j, tamanio = tamanioArray(io_id);
				for(j = 0; j < tamanio; j++){
					t_pedido* pedido = list_get(dispositivos[j].cola_dispositivo->elements, j);
					if(pedido->elemento == elemento) pedido->elemento = NULL;
				}
				tamanio = tamanioArray(sem_id);
				for(j = 0; j < tamanio; j++) list_remove_by_condition(semaforosGlobales[j].cola_bloqueados->elements, (void*)compararPid);
				list_add(listaPCBExit, elemento);
				i--;
			}
		}

		//cola ready
		pthread_mutex_lock(&mutexColaReady);
		while(colaPCBReady->elements->elements_count && hayCPUsLibres()){
			elemento = queue_pop(colaPCBReady);
			if(elemento->estado == EXEC){
				queue_push(colaPCBExec, elemento);
				cpu = cpuLibre();
				cpu->elemento = elemento;
				void* mensaje;
				int tamanioSerializacion = serializarCargarPCB(elemento->pcb, &mensaje);
				enviar(cpu->socketCPU, mensaje, tamanioSerializacion);
				free(mensaje);
			}else if(elemento->estado == EXIT){
				pidABuscar = elemento->pcb->pid;
				list_remove_by_condition(colaPCBExec->elements, (void*)compararPid);
				list_add(listaPCBExit, elemento);
			}
		}
		pthread_mutex_unlock(&mutexColaReady);

		//cola exit
		list_clean_and_destroy_elements(listaPCBExit, (void*)matarProceso);
	}
}

void matarProceso(t_elemento_cola* elemento){
	log_info(logger, "Destruyendo proceso: pid: %d", elemento->pcb->pid);
	void* mensaje;
	int tamanioSerializacion = serializarFinalizar(elemento->pcb->pid, &mensaje);
	enviar(socket_umc, mensaje, tamanioSerializacion);
	int socket = buscarSocketConsola(elemento->pcb->pid);
	if(socket != -1){
		tamanioSerializacion = serializarTerminar(&mensaje);
		enviar(buscarSocketConsola(elemento->pcb->pid), mensaje, tamanioSerializacion);
		free(mensaje);
	}
	pcb_destroy(elemento->pcb);
	//free(elemento);
}

_Bool estaLibre(t_cpu* cpu){
		return !cpu->elemento;
}

t_cpu* cpuLibre(){
	log_info(logger, "Buscando CPU libre");
	return list_find(cpus, (void*)estaLibre);
}

_Bool hayCPUsLibres(){
	log_info(logger, "Chequeando si hay CPUs libres");
	return list_any_satisfy(cpus, (void*)estaLibre);
}
