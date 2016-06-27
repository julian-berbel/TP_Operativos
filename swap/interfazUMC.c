#include "interfazUMC.h"

void (*deserializadores[5])(void*, void*) = {deserializarInicializar, terminar, deserializarLeerPagina, deserializarEscribirPagina, deserializarFinalizar};

void deserializarInicializar(void* parametrosSerializados, void* dataAdicional){
	int id_programa, paginas_requeridas;
	char* programa;

	id_programa = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	paginas_requeridas = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	programa = parametrosSerializados;

	log_info(logger, "Deserializado: Inicializar: id_programa: %d, paginas_requeridas: %d", id_programa, paginas_requeridas);
	inicializar(id_programa, paginas_requeridas, programa);
}

void deserializarLeerPagina(void* parametrosSerializados, void* dataAdicional){
	int id_programa, num_pagina;

	id_programa = *((int*) parametrosSerializados);
	parametrosSerializados += sizeof(int);

	num_pagina = *((int*) parametrosSerializados);

	log_info(logger, "Deserializado: Leer Pagina: id_programa: %d, num_pagina: %d", id_programa, num_pagina);
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

	log_info(logger, "Deserializado: Escribir Pagina: id_programa: %d, num_pagina: %d, buffer: %s", id_programa, num_pagina, buffer);
	escribir_pagina(id_programa, num_pagina, buffer);
}

void deserializarFinalizar(void* parametrosSerializados, void* dataAdicional){
	int id_programa;
	id_programa = *((int*) parametrosSerializados);

	log_info(logger, "Deserializado: Finalizar: id_programa: %d", id_programa);
	finalizar(id_programa);
}

void procesarMensaje(void* mensaje, void* dataAdicional){
	log_info(logger, "Procesando Mensaje");
	interfazPropia tipo = *((interfazPropia*) mensaje);
	void* aux = mensaje + sizeof(interfazPropia);

	(*deserializadores[tipo])(aux, dataAdicional);
	free(mensaje);
}
