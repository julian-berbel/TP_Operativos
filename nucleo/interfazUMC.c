#include "interfazUMC.h"

int serializarInicializar(int id_programa, int paginas_requeridas, char* programa, void** serializacion){
	int tamanioSerializacion = sizeof(tipoDeMensaje) + sizeof(int) * 2 + sizeof(char) * (string_length(programa) + 1);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	memset(*serializacion, 0, tamanioSerializacion);

	*((tipoDeMensaje*) aux) = INICIALIZAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = id_programa;
	aux += sizeof(int);
	*((int*) aux) = paginas_requeridas;
	aux += sizeof(int);
	memcpy(aux, programa, sizeof(char) * string_length(programa));

	return tamanioSerializacion;
}

int serializarFinalizar(int id_programa, void** serializacion){
	int tamanioSerializacion = sizeof(tipoDeMensaje) + sizeof(int);
	*serializacion = malloc(tamanioSerializacion);
	void* aux = *serializacion;

	*((tipoDeMensaje*) aux) = FINALIZAR;
	aux += sizeof(tipoDeMensaje);
	*((int*) aux) = id_programa;

	return tamanioSerializacion;
}
