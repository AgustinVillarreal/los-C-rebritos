#ifndef INCLUDE_INTERFAZ_MEMORIA_H_
#define INCLUDE_INTERFAZ_MEMORIA_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/list.h>

int reservar_espacio_memoria(int*);
int liberar_espacio_memoria(char*, int*);
int leer_espacio_memoria(char*);
int escribir_espacio_memoria();

#endif
