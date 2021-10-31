#ifndef INCLUDE_INTERFAZ_MEMORIA_H_
#define INCLUDE_INTERFAZ_MEMORIA_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/list.h>

#include "estructuras.h"
#include "manejo_memoria.h"

int reservar_espacio_mp(size_t size_stream, unsigned long id);
int liberar_espacio_mp(char*, int* size);
int leer_espacio_mp(char* size);
int escribir_espacio_mp();



#endif
