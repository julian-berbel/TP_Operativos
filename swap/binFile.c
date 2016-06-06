/*
 * binFile.c
 *
 *  Created on: 18/5/2016
 *      Author: utnso
 */
#include"swap.h"
#include"binFile.h"

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

void escribirArchivoBinario(char* programa){

	char *ruta_archivo = strcpy(ruta_archivo,RUTA_BINFILE );
	ruta_archivo = strcat(ruta_archivo,nombre_data);

	archivo = fopen(ruta_archivo,"w");

	fputs(programa,archivo);

	fclose(archivo);
}

