#ifndef MANEJO_SWAMP_H
#define  MANEJO_SWAMP_H


#include <stdint.h>
#include <stdlib.h>
#include "config.h"
#include "estructuras.h"



uint32_t cantidad_de_espacio_swamp_libre(void* swamp);
void* swamp_con_mas_espacio();

void tomar_frame_swap(unsigned long id_carpincho, uint32_t nro_pagina, void* dest);
uint32_t posicion_primer_byte_libre(void* data);
void insertar_global(void* data , void* swap);

#endif