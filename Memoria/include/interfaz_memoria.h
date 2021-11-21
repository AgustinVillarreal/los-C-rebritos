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

bool allocar_carpincho(unsigned long id_carpincho, size_t size, uint32_t* direccion_logica);
uint32_t reservar_espacio_mp(size_t size_stream, unsigned long id, t_list* tabla_carpincho, bool primerPagina);
uint32_t liberar_espacio_mp(unsigned long id_carpincho, uint32_t* direccion_logica);
uint32_t leer_espacio_mp(char* size);
uint32_t escribir_espacio_mp();
void mate_init(unsigned long id);

uint32_t liberar_espacio_mp(unsigned long id_carpincho, uint32_t* direccion_logica);


#endif
