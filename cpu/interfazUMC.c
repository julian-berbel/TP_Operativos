#include "interfazUMC.h"

int serializarSolicitar(int num_pagina, int offset, size_t t, void** serializacion){
	int tamanioSerializacion = sizeof(tipoDeMensaje) + sizeof(int) * 3;
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	*((tipoDeMensaje*) aux) = SOLICITAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = num_pagina;
	aux += sizeof(int);
	*((int*) aux) = offset;
	aux += sizeof(int);
	*((int*) aux) = t;

	return tamanioSerializacion;
}

int serializarAlmacenar(int num_pagina, int offset, size_t t, char* buffer, void** serializacion){
	int tamanioSerializacion = sizeof(tipoDeMensaje) + sizeof(int) * 3 + sizeof(char) * (string_length(buffer) + 1);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	memset(*serializacion, 0, tamanioSerializacion);

	*((tipoDeMensaje*) aux) = ALMACENAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = num_pagina;
	aux += sizeof(int);
	*((int*) aux) = offset;
	aux += sizeof(int);
	*((int*) aux) = t;
	aux += sizeof(int);
	memcpy(aux, buffer, sizeof(char) * string_length(buffer));

	return tamanioSerializacion;
}
