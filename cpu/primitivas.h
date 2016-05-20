/*
 * primitivas.h
 *
 *  Created on: 12/5/2016
 *      Author: utnso
 */

#ifndef PRIMITIVAS_H_
#define PRIMITIVAS_H_

#include <parser/parser.h>

static const int CONTENIDO_VARIABLE = 20;
static const int POSICION_MEMORIA = 0x10;

t_puntero definirVariable(t_nombre_variable variable);
t_puntero obtenerPosicionVariable(t_nombre_variable variable);
t_valor_variable dereferenciar(t_puntero puntero);
void asignar(t_puntero puntero, t_valor_variable variable);
void imprimir(t_valor_variable valor);
void imprimirTexto(char* texto);

#endif /* PRIMITIVAS_H_ */
