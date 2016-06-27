#include "interfazNucleoCPU.h"

void (*deserializadores[6])(void*, void*) = {deserializarInicializar, terminar, deserializarSolicitar, deserializarAlmacenar, deserializarFinalizar, deserializarCambiarProcesoActivo};

void deserializarInicializar(void* parametrosSerializados, void* cliente){
	int id_programa, paginas_requeridas;
	char* programa;

	id_programa = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	paginas_requeridas = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	programa = parametrosSerializados;

	log_info(logger, "Deserializado: inicializar: id_programa: %d, paginas_requeridas: %d", id_programa, paginas_requeridas);
	inicializar(id_programa, paginas_requeridas, programa, cliente);
}

void deserializarSolicitar(void* parametrosSerializados, void* cliente){
	int num_pagina, offset;
	size_t t;

	num_pagina = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	offset = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	t = *((int*) parametrosSerializados);

	log_info(logger, "Deserializado: Leer Pagina: num_pagina: %d, offset: %d, t: %d", num_pagina, offset, t);
	leer_pagina(num_pagina, offset, t, cliente);
}

void deserializarAlmacenar(void* parametrosSerializados, void* cliente){
	int num_pagina, offset;
	size_t t;
	char* buffer;

	num_pagina = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	offset = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	t = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	buffer = parametrosSerializados;

	log_info(logger, "Deserializado: Escribir Pagina: num_pagina: %d, offset: %d, t: %d, buffer: %s", num_pagina, offset, t, buffer);
	escribir_pagina(num_pagina, offset, t, buffer, cliente);
}

void deserializarFinalizar(void* parametrosSerializados, void* cliente){
	int id_programa;
	id_programa = *((int*) parametrosSerializados);

	log_info(logger, "Deserializado: Finalizar: id_programa: %d", id_programa);
	finalizar(id_programa);
}

void deserializarCambiarProcesoActivo(void* parametrosSerializados, void* cliente){
	int pid = *(int*) parametrosSerializados;

	log_info(logger, "Deserializado: Cambiar Proceso Activo: pid: %d", pid);
	cambiar_proceso_activo(pid, cliente);
}

void procesarMensaje(void* mensaje, void* cliente){
	log_info(logger, "Procesando Mensaje");
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux, cliente);
	free(mensaje);
}
