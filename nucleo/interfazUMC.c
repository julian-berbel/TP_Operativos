#include "interfazUMC.h"

int serializarInicializar(int id_programa, int paginas_requeridas, char* programa, void** serializacion){
	int tamanioSerializacion = sizeof(interfazUMC) + sizeof(int) * 2 + sizeof(char) * (string_length(programa) + 1);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	memset(*serializacion, 0, tamanioSerializacion);

	*((interfazUMC*) aux) = INICIALIZAR;
	aux += sizeof(interfazUMC);
	*((int*) aux) = id_programa;
	aux += sizeof(int);
	*((int*) aux) = paginas_requeridas;
	aux += sizeof(int);
	memcpy(aux, programa, sizeof(char) * string_length(programa));

	return tamanioSerializacion;
}

int serializarFinalizar(int id_programa, void** serializacion){
	int tamanioSerializacion = sizeof(interfazUMC) + sizeof(int);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	*((interfazUMC*) aux) = FINALIZAR;
	aux += sizeof(interfazUMC);
	*((int*) aux) = id_programa;

	return tamanioSerializacion;
}
