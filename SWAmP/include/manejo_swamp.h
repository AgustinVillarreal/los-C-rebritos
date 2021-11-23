#ifndef MANEJO_SWAMP_H
#define  MANEJO_SWAMP_H


#include <stdint.h>
#include <stdlib.h>
#include "config.h"
#include "estructuras.h"



uint32_t cantidad_de_espacio_swamp_libre(uint32_t swamp);
uint32_t swamp_con_mas_espacio();

void* tomar_frame_swap(unsigned long id_carpincho, uint32_t nro_pagina);
uint32_t posicion_primer_byte_libre(void* data);
void insertar_global(void* data , void* swap);
bool hay_marcos_libres(t_list* list);
uint32_t primer_byte_no_asignado(uint32_t nro_swap);

#endif