#ifndef MANEJO_SWAMP_H
#define  MANEJO_SWAMP_H


#include <stdint.h>
#include <stdlib.h>
#include "config.h"
#include "estructuras.h"



uint32_t cantidad_de_espacio_swamp_libre(uint32_t swamp);
uint32_t swamp_con_mas_espacio();
void* tomar_frame_swap(unsigned long id_carpincho, uint32_t nro_pagina);
uint32_t posicion_primer_byte_libre(uint32_t data);
bool hay_marcos_disponibles(unsigned long id, uint32_t a_allocar);
void formatear_pagina_swap(uint32_t inicio,uint32_t nro_swap);
void eliminar_carpincho_de_memoria(unsigned long id_carpincho);

#endif