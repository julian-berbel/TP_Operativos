/*
 * binFile.h
 *
 *  Created on: 18/5/2016
 *      Author: utnso
 */

#ifndef BINFILE_H_
#define BINFILE_H_

#include<stdio.h>
#include<commons/log.h>
#include<commons/config.h>

#define RUTA_BINFILE "/home/utnso/tp-2016-1c-Hellfish-Group/swap/"

FILE *archivo;

void crearArchivoBinario(char* nombre_data,int pagina_size,int cant_paginas);
char* tamanioArchivo(int pag_size,int cantidad);
void escribirArchivoBinario(char* programa);

#endif /* BINFILE_H_ */
