#include "interfazUMC.h"

void (*deserializadores[5])(void*, void*) = {terminar, deserializarInicializar, deserializarLeerPagina, deserializarEscribirPagina, deserializarFinalizar};

void deserializarInicializar(void* parametrosSerializados, void* dataAdicional){
	int id_programa, paginas_requeridas;
	char* programa;

	id_programa = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	paginas_requeridas = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	programa = parametrosSerializados;

	inicializar(id_programa, paginas_requeridas, programa);
}

void deserializarLeerPagina(void* parametrosSerializados, void* dataAdicional){
	int id_programa, num_pagina;

	id_programa = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	num_pagina = *((int*) parametrosSerializados);

	leer_pagina(id_programa, num_pagina);
}

void deserializarEscribirPagina(void* parametrosSerializados, void* dataAdicional){
	int id_programa, num_pagina;
	char* buffer;

	id_programa = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	num_pagina = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	buffer = parametrosSerializados;

	escribir_pagina(id_programa, num_pagina, buffer);
}

void deserializarFinalizar(void* parametrosSerializados, void* dataAdicional){
	int id_programa;
	id_programa = *((int*) parametrosSerializados);

	finalizar(id_programa);
}

void procesarMensaje(void* mensaje, void* dataAdicional){
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux, dataAdicional);
	free(mensaje);
}
