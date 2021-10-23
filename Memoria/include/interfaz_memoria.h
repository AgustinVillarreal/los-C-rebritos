#ifndef INCLUDE_INTERFAZ_MEMORIA_H_
#define INCLUDE_INTERFAZ_MEMORIA_H_

#include <stdio.h>

int reservar_espacio_memoria(int*);
int liberar_espacio_memoria(char*, int*);
int leer_espacio_memoria(char*);
int escribir_espacio_memoria();

#endif
