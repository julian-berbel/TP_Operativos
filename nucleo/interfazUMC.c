#include "interfazUMC.h"

int serializarInicializar(int id_programa, int paginas_requeridas, char* programa, void** serializacion){
	log_info(logger, "Serializando: Inicializar: id: %d, paginas_requeridas: %d", id_programa, paginas_requeridas, programa);
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
	log_info(logger, "Serializando: Finalizar: id: %d", id_programa);
	int tamanioSerializacion = sizeof(interfazUMC) + sizeof(int);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	*((interfazUMC*) aux) = FINALIZAR;
	aux += sizeof(interfazUMC);
	*((int*) aux) = id_programa;

	return tamanioSerializacion;
}
