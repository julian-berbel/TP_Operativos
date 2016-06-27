#include "interfazSwap.h"

int serializarTerminar(void** serializacion) {
	log_info(logger, "Serializando: Terminar");
	*serializacion = malloc(sizeof(interfazSwap));
	*(interfazSwap*)*serializacion = TERMINAR;

	return sizeof(interfazSwap);
}

int serializarInicializar(int id_programa, int paginas_requeridas, char* programa, void** serializacion){
	log_info(logger, "Serializando: Inicializar: id_programa: %d, paginas_requeridas: %d", id_programa, paginas_requeridas);
	int tamanioSerializacion = sizeof(interfazSwap) + sizeof(int) * 2 + sizeof(char) * (string_length(programa) + 1);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	memset(*serializacion, 0, tamanioSerializacion);

	*((interfazSwap*) aux) = INICIALIZAR;
	aux += sizeof(interfazSwap);
	*((int*) aux) = id_programa;
	aux += sizeof(int);
	*((int*) aux) = paginas_requeridas;
	aux += sizeof(int);
	memcpy(aux, programa, sizeof(char) * string_length(programa));

	return tamanioSerializacion;
}

int serializarLeerPagina(int id_programa, int num_pagina, void** serializacion){
	log_info(logger, "Serializando: Leer Pagina: id_programa: %d, num_pagina: %d", id_programa, num_pagina);
	int tamanioSerializacion = sizeof(interfazSwap) + sizeof(int) * 2;
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	*((interfazSwap*) aux) = LEER_PAGINA;
	aux += sizeof(interfazSwap);
	*((int*) aux) = id_programa;
	aux += sizeof(int);
	*((int*) aux) = num_pagina;

	return tamanioSerializacion;
}

int serializarEscribirPagina(int id_programa, int num_pagina, char* buffer, void** serializacion){
	log_info(logger, "Serializando: Escribir Pagina: id_programa: %d, num_pagina: %d, buffer: %s", id_programa, num_pagina, buffer);
	int tamanioSerializacion = sizeof(interfazSwap) + sizeof(int) * 2 + sizeof(char) * (string_length(buffer) + 1);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	memset(*serializacion, 0, tamanioSerializacion);

	*((interfazSwap*) aux) = ESCRIBIR_PAGINA;
	aux += sizeof(interfazSwap);
	*((int*) aux) = id_programa;
	aux += sizeof(int);
	*((int*) aux) = num_pagina;
	aux += sizeof(int);
	memcpy(aux, buffer, sizeof(char) * string_length(buffer));

	return tamanioSerializacion;
}

int serializarFinalizar(int id_programa, void** serializacion){
	log_info(logger, "Serializando: Finalizar: id_programa: %d", id_programa);
	int tamanioSerializacion = sizeof(interfazSwap) + sizeof(int);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	*((interfazSwap*) aux) = FINALIZAR;
	aux += sizeof(interfazSwap);
	*((int*) aux) = id_programa;

	return tamanioSerializacion;
}
