/*
 * binFile.c
 *
 *  Created on: 18/5/2016
 *      Author: utnso
 */

#include"binFile.h"

char* rutaArchivoSwap() {
	char *ruta_archivo = string_new();
	string_append(&ruta_archivo, RUTA_BINFILE);
	string_append(&ruta_archivo, nombre_data);

	return ruta_archivo;
}

long int paginaEnBytes(int numPagina) {
	return numPagina * pagina_size;
}

void crearArchivoBinario(char* nombre_data, int pagina_size, int cant_paginas) {
	char* comando = string_new();
	string_append(&comando, "dd if=/dev/zero ");
	string_append(&comando, "of=");
	string_append(&comando, RUTA_BINFILE);
	string_append(&comando, nombre_data);
	string_append(&comando, " bs=");
	string_append(&comando, tamanioArchivo(pagina_size, cant_paginas));
	string_append(&comando, " count=1");

	system(comando);

}

char* tamanioArchivo(int pagina_tamanio, int cantidad_pags) {
	int tamanioTotal;
	static char tamanioTotalString[10]; //hasta 9 cifras de tamaño

	tamanioTotal = pagina_tamanio * cantidad_pags;

	sprintf(tamanioTotalString, "%d", tamanioTotal);
	return tamanioTotalString;
}

void escribirArchivoBinario(char* programa) { //testeado

	usleep(retardo_acceso * 1000);

	archivo = fopen(rutaArchivoSwap(), "a");

	if(archivo == NULL){
		printf("error");
	}

	fputs(programa, archivo);

	fclose(archivo);
}

void escribirArchivoBinarioEnPag(int numPagina, char* buffer) {

	usleep(retardo_acceso * 1000);

	archivo = fopen(rutaArchivoSwap(), "wb");

	fseek(archivo, paginaEnBytes(numPagina), SEEK_SET);

	fputs(buffer, archivo);

	fclose(archivo);
}

char* leerArchivoBinarioEnPagina(int numPagina) {

	usleep(retardo_acceso * 1000);
	archivo = fopen(rutaArchivoSwap(), "rb");
	char* buffer = malloc(pagina_size);
	fseek(archivo, paginaEnBytes(numPagina), SEEK_SET);
	fread(buffer, pagina_size, 1, archivo);
	fclose(archivo);

	return buffer;

}
