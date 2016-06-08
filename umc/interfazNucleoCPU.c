#include "interfazNucleoCPU.h"

void (*deserializadores[5])(void*) = {terminar, deserializarInicializar, deserializarSolicitar, deserializarAlmacenar, deserializarFinalizar};

void deserializarInicializar(void* parametrosSerializados){
	int id_programa, paginas_requeridas;
	char* programa;

	id_programa = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	paginas_requeridas = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	programa = parametrosSerializados;

	inicializar(id_programa, paginas_requeridas, programa);
}

void deserializarSolicitar(void* parametrosSerializados){
	int num_pagina, offset;
	size_t t;

	num_pagina = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	offset = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	t = *((int*) parametrosSerializados);

	leer_pagina(num_pagina, offset, t);
}

void deserializarAlmacenar(void* parametrosSerializados){
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

	escribir_pagina(num_pagina, offset, t, buffer);
}

void deserializarFinalizar(void* parametrosSerializados){
	int id_programa;
	id_programa = *((int*) parametrosSerializados);

	finalizar(id_programa);
}

void procesarMensaje(void* mensaje){
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux);
	free(mensaje);
}
