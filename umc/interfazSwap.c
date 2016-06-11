#include "interfazSwap.h"

int serializarTerminar(void** serializacion) {
	*serializacion = malloc(sizeof(interfazSwap));
	*(interfazSwap*)*serializacion = TERMINAR;

	return sizeof(interfazSwap);
}

int serializarInicializar(int id_programa, int paginas_requeridas, char* programa, void** serializacion){
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
	int tamanioSerializacion = sizeof(interfazSwap) + sizeof(int);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	*((interfazSwap*) aux) = FINALIZAR;
	aux += sizeof(interfazSwap);
	*((int*) aux) = id_programa;

	return tamanioSerializacion;
}
