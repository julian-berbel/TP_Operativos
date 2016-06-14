#include "interfazUMC.h"

int serializarSolicitar(int num_pagina, int offset, size_t t, void** serializacion){
	int tamanioSerializacion = sizeof(interfazUMC) + sizeof(int) * 3;
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	*((interfazUMC*) aux) = SOLICITAR;
	aux += sizeof(interfazUMC);
	*((int*) aux) = num_pagina;
	aux += sizeof(int);
	*((int*) aux) = offset;
	aux += sizeof(int);
	*((int*) aux) = t;

	return tamanioSerializacion;
}

int serializarAlmacenar(int num_pagina, int offset, size_t t, char* buffer, void** serializacion){
	int tamanioSerializacion = sizeof(interfazUMC) + sizeof(int) * 3 + sizeof(char) * (string_length(buffer) + 1);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	memset(*serializacion, 0, tamanioSerializacion);

	*((interfazUMC*) aux) = ALMACENAR;
	aux += sizeof(interfazUMC);
	*((int*) aux) = num_pagina;
	aux += sizeof(int);
	*((int*) aux) = offset;
	aux += sizeof(int);
	*((int*) aux) = t;
	aux += sizeof(int);
	memcpy(aux, buffer, sizeof(char) * string_length(buffer));

	return tamanioSerializacion;
}

int serializarCambioDeProcesoActivo(int pid, void** serializacion){
	int tamanioSerializacion = sizeof(interfazUMC) + sizeof(int);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	*((interfazUMC*) aux) = CAMBIAR_PROCESO_ACTIVO;
	aux += sizeof(interfazUMC);
	*((int*) aux) = pid;

	return tamanioSerializacion;
}
