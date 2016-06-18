#include "interfazNucleoCPU.h"

void (*deserializadores[6])(void*, void*) = {deserializarInicializar, terminar, deserializarSolicitar, deserializarAlmacenar, deserializarFinalizar, deserializarCambiarProcesoActivo};

void deserializarInicializar(void* parametrosSerializados, void* dataAdicional){
	int id_programa, paginas_requeridas;
	char* programa;

	id_programa = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	paginas_requeridas = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	programa = parametrosSerializados;

	inicializar(id_programa, paginas_requeridas, programa, dataAdicional);
}

void deserializarSolicitar(void* parametrosSerializados, void* dataAdicional){
	int num_pagina, offset;
	size_t t;

	num_pagina = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	offset = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	t = *((int*) parametrosSerializados);

	leer_pagina(num_pagina, offset, t, dataAdicional);
}

void deserializarAlmacenar(void* parametrosSerializados, void* dataAdicional){
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

	escribir_pagina(num_pagina, offset, t, buffer, dataAdicional);
}

void deserializarFinalizar(void* parametrosSerializados, void* dataAdicional){
	int id_programa;
	id_programa = *((int*) parametrosSerializados);

	finalizar(id_programa);
}

void deserializarCambiarProcesoActivo(void* parametrosSerializados, void* dataAdicional){
	int pid = *(int*) parametrosSerializados;
	cambiar_proceso_activo(pid, dataAdicional);
}

void procesarMensaje(void* mensaje, void* dataAdicional){
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux, dataAdicional);
	free(mensaje);
}
